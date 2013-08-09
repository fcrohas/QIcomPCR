var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped',
		maxBufferSize : 110250, // 5s ring buffer
		isApplet : false,
		resample : false,
		directplay: false
    },
    initialize: function() { 
		this.playing = false;
		this.finished = false;
		this.directplay = this.get("directplay");
		window.AudioContext = window.AudioContext || window.webkitAudioContext;
		if ( (window.AudioContext == null) || (window.AudioContext == undefined)) {
			this.set('isApplet',true);
		}
		this.maxBufferSize = this.get('maxBufferSize');
		if (this.get('isApplet') == false) {
			// ******************  HTML5 VERSION *****************
			this.codec = new Speex({
				benchmark: false
				, quality: 8
				, enh : 1
				, complexity : 2
			});
			this.context = new window.AudioContext();
			this.source = this.context.createBufferSource(0); // creates a sound source    
			this.source.onended = this.playBufferEnded;
			this.gainNode = this.context.createGain();
			this.gainNode.gain.value = 1.0;
			this.buffer = this.context.createBuffer(2, 16384, 22050); //  5s buffer	
			if (this.get('resample') == true) {
			  this.resampleControl = new Resampler(11025,22050,1,512,true);
			}
			// Build a filler thread to fill audiobuffer loop after each playing
			if (this.directplay == false) {
				this.filler = this.context.createScriptProcessor(16384);
				this.filler.onaudioprocess = this.getRingBufferData;
			}
			// for ring buffer allocate twice the size
			this.ringbuffer = new Float32Array(this.maxBufferSize*2); 
			// Current ring buffer size
			this.ringsize = 0;
			// Current ring buffer reader position
			this.ringoffset = 0;
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
		// Decode Speex buffer
		var decoded = this.codec.decode(audioData); // to Int16 decoding buffer
		if ((decoded ==null) || (decoded == undefined))
		      return;
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
		if (this.ringsize >= this.maxBufferSize) {
			// Copy remaining to buffer start
			for (var i=this.maxBufferSize,j=0; i < this.ringsize; i++,j++) {
				this.ringbuffer[j] = this.ringbuffer[i];
				this.ringsize = j+1;	
			}
		}
		if (this.directplay == true) {
			//this.playDirectBuffer();
		}
		if ((this.playing == false) && (this.ringsize >=this.maxBufferSize/5)) {
		    // wait for enough buffer
		    this.playBuffer();
		    this.playing = true;
		}
		//delete audioData;
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
			this.filler.connect(this.context.destination);
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
	    // Check if enought byte to fill audio buffer
	    if (this.main.ringAvailableBytes >= output.length/2) { // this is stereo buffer so half data
		    // Fill audio buffer for next play
		    var curpos = 0;
		    var j=0;
		    for (var i = 0; i < output.length/2; i++)
		    {
			    // if end of ring buffer not reached
			    if (this.main.ringoffset+i<this.main.maxBufferSize) {
				    output[j] = this.main.ringbuffer[this.main.ringoffset+i];
				    output[j+1] = this.main.ringbuffer[this.main.ringoffset+i];
				    curpos = i;
			    } else {
				    // else get the rest from pos 0 of buffer
				    output[j] = this.main.ringbuffer[i-(curpos+1)];
				    output[j+1] = this.main.ringbuffer[i-(curpos+1)];
			    }
			    j+=2;
		    }
		    // Adjust new offset in ring buffer
		    this.main.ringoffset += output.length/2+1;
		    if (this.main.ringoffset >= this.main.maxBufferSize)
			    this.main.ringoffset = this.main.ringoffset - this.main.maxBufferSize;
		    // Decrease available byte read from buffer
		    this.main.ringAvailableBytes -= output.length/2;
			this.main.finished = false;
	    }
    },
	playBufferEnded: function(event) {
		this.main.finished = true;
		console.log("play finished");
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
				  if (sound.ringoffset+i<sound.maxBufferSize) {
					  buf[i] = sound.ringbuffer[sound.ringoffset+i];
					  curpos = i;
				  } else {
					  // else get the rest from pos 0 of buffer
					  buf[i] = sound.ringbuffer[i-(curpos+1)];				
				  }
			  }
			  // Adjust new offset in ring buffer
			  sound.ringoffset += buf.length+1;
			  if (sound.ringoffset >= sound.maxBufferSize)
				  sound.ringoffset = sound.ringoffset - sound.maxBufferSize;
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
    }
});	
