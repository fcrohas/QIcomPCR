var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped',
		volume: 1.0,
		maxBufferSize : 110250, //524288,//110250, // 5s ring buffer
		isApplet : false,
		resample : false,
		directplay: false,
		testBuffer: false
    },
    initialize: function() { 
		this.playing = false;
		this.finished = false;
		this.sampleBuffer = 8192;
		this.on('change:volume', this.setVolume);
		this.directplay = this.get("directplay");
		this.testBuffer = this.get("testBuffer");
		window.AudioContext = window.AudioContext || window.webkitAudioContext;
		if ( (window.AudioContext == null) || (window.AudioContext == undefined)) {
			this.set('isApplet',true);
		}
		this.maxBufferSize = this.get('maxBufferSize');
		if (this.get('isApplet') == false) {
			// ******************  HTML5 VERSION *****************
			this.codec = new Speex({
				benchmark: false
				, quality: 5
				, enh : 1
				, complexity : 2
			});
			this.context = new window.AudioContext();
			this.source = this.context.createBufferSource(0); // creates a sound source    
			this.source.onended = this.playBufferEnded;
			this.gainNode = this.context.createGain();
			this.gainNode.gain.value = this.get("volume");
			this.buffer = this.context.createBuffer(2, this.sampleBuffer, 22050); //  5s buffer	
			if (this.get('resample') == true) {
			  this.resampleControl = new Resampler(11025,22050,1,512,true);
			}
			// Build a filler thread to fill audiobuffer loop after each playing
			if (this.directplay == false) {
				this.filler = this.context.createScriptProcessor(this.sampleBuffer);
				this.filler.onaudioprocess = this.getRingBufferData;
			}
			// for ring buffer allocate maxbufferSize  + one sample buffer 
			this.ringbuffer = new Float32Array(this.maxBufferSize+this.sampleBuffer); 
			// Current ring buffer size
			this.ringsize = 0;
			// Current ring buffer reader position
			this.RingOffset = 0;
			// Current number of byte available in ring buffer
			this.ringAvailableBytes = 0;
			this.starttime = 0;
			this.source.main = this;
			if (this.directplay == false) {			
				this.filler.main = this;		
			}			
		}
    },
    disconnect: function() {
      if (this.get('isApplet') == false) {
		codec.close();
		this.source.stop();
      }
    },
    // Socket.IO management
    connect: function() {
		this.socket = new BinaryClient('ws://'+window.location.host+this.get('path'));
		this.socket.on('open', this.onConnect);
		this.socket.on('stream', this.onStream);
		this.audio = null;
		if (this.get('isApplet') == true) {
			// ******************  APPLET VERSION *****************
			// ****************** See SpeexAudio.jar **************
			// Use SpeexAudio Applet
			var attributes = { id:'SpeexAudio', code:'com.lilisoft.Main.class',  width:100, height:50};
			var parameters = {jnlp_href: 'SpeexAudio.jnlp'} ;
			deployJava.runApplet(attributes, parameters, '1.6');		
			this.audio = SpeexAudio;
			// ****************** See SpeexAudio.jar ************** 
		}
		this.socket.model = this;		
    },
    onConnect: function() {
      this.on('disconnect', this.model.onDisconnect);      
    },
    onStream: function(stream,meta)  {
      // WebSocket stream received
      stream.audio = this.model.audio;
      stream.model = this.model;
      stream.on('data', function(data) {

	    if (this.model.get('isApplet') == false) {      
			// ******************  HTML5 VERSION *****************
			this.model.addToRingBuffer(data);
	    } else {
			// ******************  APPLET VERSION *****************
			// ****************** See SpeexAudio.jar **************
			// Send received speex data to audio applet      
			this.model.audio.getAudio(new Int8Array(data));
			// ****************** See SpeexAudio.jar **************
	    }
      });
      this.model.set('state', 'Playing');	
    },
    onDisconnect : function() {
      this.model.set('state', 'Stopped');	
    },
    addToRingBuffer : function(data) {
		// Convert ArrayBuffer to Int8Array
		var audioData = new Int8Array(data);
		var decoded = null;
		// Decode Speex buffer
    	if (this.testBuffer == false) {
			decoded = this.codec.decode(audioData); // to Int16 decoding buffer
			if ((decoded ==null) || (decoded == undefined))
			      return;
		} else {
			// create loop data
			decoded = new Float32Array(128);
			var j=1;
			for (var i=0; i<decoded.length;i++) {
				decoded[i] = j;
				j++;
				if (j>4) j=1;
			}
		}
		// resample
		if (this.get('resample') == true) {
		  var resampleLength = this.resampleControl.resampler(decoded);
		  for (var i=0; i < resampleLength; i++) {
			  this.ringbuffer[i + this.ringsize] = this.resampleControl.outputBuffer[i];
		  }
		  this.ringAvailableBytes += resampleLength;
  		  this.ringsize += resampleLength;
		} else {
		  for (var i=0; i < decoded.length; i++) {
			  this.ringbuffer[i + this.ringsize] = decoded[i];
		  }
		  this.ringAvailableBytes += decoded.length; 
		  this.ringsize += decoded.length;		  
		}
		// Ring buffer size reach the limit
		// move over maxbuffersize back to start of ring buffer
		if (this.ringsize > this.maxBufferSize) {
			// Copy remaining to buffer start
			var max = 0
			for (var i=this.maxBufferSize,j=0; i < this.ringsize; i++,j++) {
				this.ringbuffer[j] = this.ringbuffer[i];
				this.ringbuffer[i] = 0;
				max = j+1;	
			}
			this.ringsize = max;
		}
		if ((this.playing == false) && (this.ringsize >this.maxBufferSize/5)) {
		    // wait for enough buffer
		    this.playBuffer();
		    this.playing = true;
		}
		if (this.ringAvailableBytes> this.maxBufferSize) this.ringAvailableBytes = this.maxBufferSize; // limit is buffer size
		/*
		if (this.testBuffer == true) {
			var data="addToRingBuffer:";
			for(var i=0; i < this.ringbuffer.length;i++) {
				data+=this.ringbuffer[i];
			}
			console.log(data);
		}*/
    },
    playBuffer : function() {
		// Get buffer pointer to channel 0
		var buf = this.buffer.getChannelData(0);
		// Clear audio buffer on start
		for (var i=0; i<buf.length;i++) {
		    buf[i] = 0;
		}
		// Assign buffer
		this.source.buffer = this.buffer;   // tell the source which sound to play
		// Connect a filler process for audio buffer
		if (this.directplay == false) {	
			this.source.connect(this.filler);
			this.filler.connect(this.gainNode);
			this.gainNode.connect(this.context.destination);
			// Do loop audio buffer
			this.source.loop = false;
		}	else {
			// Connect the source to the context's destination (the speakers)    
			// Connect to destination
			this.source.connect(this.gainNode);
			this.gainNode.connect(this.context.destination);
			// Do loop audio buffer
			this.source.loop = true;
		}
		// Add a timer to fill buffer only every buffer duration ms
		setInterval(this.playDirectBuffer, this.buffer.duration*1000);
		// Start playing buffer
		this.source.start(0); 
    },
    getRingBufferData: function(event) {
	    // Took buffer length bits for next loop
    	var output = event.outputBuffer.getChannelData(0);
	    var size = output.length / 2;
	    // Check if enought byte to fill audio buffer
	    if (this.main.ringAvailableBytes >= size) { // this is stereo buffer so half data
		    // Fill audio buffer for next play
		    var curpos = 0;
		    var j=0;
		    for (var i = 0; i < size; i++)
		    {
			    // if end of ring buffer not reached
			    if (this.main.RingOffset+i<this.main.maxBufferSize) {
				    output[j] = this.main.ringbuffer[this.main.RingOffset+i];
				    output[j+1] = this.main.ringbuffer[this.main.RingOffset+i];
				    curpos = i+1;
			    } else {
				    // else get the rest from pos 0 of buffer
				    output[j] = this.main.ringbuffer[i-curpos];
				    output[j+1] = this.main.ringbuffer[i-curpos];
			    }
			    j+=2;
		    }
		    // Adjust new offset in ring buffer to next sample
		    var prevOffset = this.main.RingOffset;
		    if (this.main.RingOffset+size >= this.main.maxBufferSize) {
		    	this.main.RingOffset = this.main.RingOffset+size - this.main.maxBufferSize;
		    } else {
		    	this.main.RingOffset += size;
		    }
			//console.log('offset set to '+ this.main.RingOffset+ ' last offset was '+prevOffset);		    
		    // Decrease available byte read from buffer
		    this.main.ringAvailableBytes -= size;
		    if (this.main.ringAvailableBytes < 0) this.main.ringAvailableBytes = 0;
			this.main.finished = false;
			/*
			if (this.main.testBuffer == true) {
				var data="getRingBufferData:";
				for(var i=0; i < output.length;i+=2) {
					data+=output[i];
				}
				console.log(data);
			}*/
	    }
    },
	playBufferEnded: function(event) {
		this.main.finished = true;
	},
	playDirectBuffer : function() {
		try {
			//this.buffer = this.context.createBuffer(1, 256, 22050); //  5s buffer	
			var buf = sound.buffer.getChannelData(0);
			if (sound.ringAvailableBytes >= buf.length) {
			  //console.log("Ring buffer size is " + this.ringsize);
			  var curpos = 0;
			  for (var i = 0; i < buf.length; i++)
			  {
				  // if end of ring buffer not reached
				  if (sound.RingOffset+i<sound.maxBufferSize) {
					  buf[i] = sound.ringbuffer[sound.RingOffset+i];
					  curpos = i;
				  } else {
					  // else get the rest from pos 0 of buffer
					  buf[i] = sound.ringbuffer[i-(curpos+1)];				
				  }
			  }
			  // Adjust new offset in ring buffer
			  sound.RingOffset += buf.length;
			  if (sound.RingOffset >= sound.maxBufferSize)
				  sound.RingOffset = sound.RingOffset - sound.maxBufferSize;
			  // Decrease available byte read from buffer
			  sound.ringAvailableBytes -= buf.length;
			  sound.source.buffer = sound.buffer;   // tell the source which sound to play
			  //this.source.loop = false;
			  //this.source.noteOn(0); 

			  sound.finished = false;
			}
		} catch (e) {
			console.log(e.message);
		}
    },
    setVolume : function() {
    	//console.log("volume="+this.get("volume"));
    	this.gainNode.gain.value = this.get("volume");
    }
});	
