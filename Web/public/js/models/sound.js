var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped',
		maxBufferSize : 110250, // 5s ring buffer
		isApplet : false,
		resample : false
    },
    initialize: function() { 
		this.playing = false;
		window.AudioContext = window.AudioContext || window.webkitAudioContext;
		if ( (window.AudioContext == null) || (window.AudioContext == undefined)) {
			this.set('isApplet',true);
		}
		this.maxBufferSize = this.get('maxBufferSize');
		if (this.get('isApplet') == false) {
			// ******************  HTML5 VERSION *****************
			this.codec = new Speex({
				benchmark: false
				, quality: 4
				, enh : 1
				, complexity : 3
				, bits_size: 24
			});
			this.context = new window.AudioContext();
			this.source = this.context.createBufferSource(0); // creates a sound source    
			this.buffer = this.context.createBuffer(1, 256, 22050); //  5s buffer	
			if (this.get('resample') == true) {
			  this.resampleControl = new Resampler(11025,22050,1,512,true);
			}
			// Build a filler thread to fill audiobuffer loop after each playing
			this.filler = this.context.createScriptProcessor(256, 1, 1); //createJavaScriptNode
			this.filler.onaudioprocess = this.getRingBufferData;
			// for ring buffer allocate twice the size
			this.ringbuffer = new Float32Array(this.maxBufferSize*2); 
			// Current ring buffer size
			this.ringsize = 0;
			// Current ring buffer reader position
			this.ringoffset = 0;
			// Current number of byte available in ring buffer
			this.ringAvailableBytes = 0;
			this.starttime = 0;
			this.filler.main = this;			
		}
    },
    disconnect: function() {
      if (this.get('isApplet') == false) {
		codec.close();
      }
    },
    // Socket.IO management
    connect: function() {
		this.socket = new BinaryClient('ws://'+window.location.host+this.get('path'));
		this.socket.model = this;
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
    },
    onConnect: function() {
      this.on('disconnect', this.model.onDisconnect);      
      console.log("connected to sound");
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
		this.audioData = new Int8Array(data);
		// Decode Speex buffer
		var decoded = this.codec.decode(this.audioData); // to Int16 decoding buffer
		// resample
		if (this.get('resample') == true) {
		  var resampleLength = this.resampleControl.resampler(decoded);
		  for (var i=0; i < resampleLength; i++) {
			  this.ringbuffer[i + this.ringsize] = this.resampleControl.outputBuffer[i];
		  }
		  this.ringAvailableBytes += resampleLength;
		} else {
		  for (var i=0; i < decoded.length; i++) {
			  this.ringbuffer[i + this.ringsize] = decoded[i];
		  }
		  this.ringAvailableBytes += decoded.length; 
		}
		//console.log(this.ringbuffer);		
		this.ringsize = this.ringsize + decoded.length;
		// Ring buffer size reach the limit
		// move over maxbuffersize back to start of ring buffer
		if (this.ringsize >= this.maxBufferSize) {
			// Copy remaining to buffer start
			for (var i=this.maxBufferSize,j=0; i <= this.ringsize; i++,j++) {
				this.ringbuffer[j] = this.ringbuffer[i];
				this.ringsize = j;	
				//this.ringAvailableBytes += this.ringsize - this.maxBufferSize;
			}
		}
		if ((this.playing == false) && (this.ringsize >=this.maxBufferSize/4)) {
		    // wait for enough buffer
		    this.playBuffer();
		    this.playing = true;
		}
		//console.log("addToRingBuffer: size="+this.ringsize+" available="+this.ringAvailableBytes+" offset="+this.ringoffset);	      		
		this.audioData = undefined;
    },
    playBuffer : function() {
		console.log("Play Buffer");      
		// Get buffer pointer to channel 0
		var buf = this.buffer.getChannelData(0);
		// Clear audio buffer on start
		for (var i=0; i<buf.length;i++) {
		    buf[i] = 0;
		}
		// Assign buffer
		this.source.buffer = this.buffer;   // tell the source which sound to play
		// Do loop audio buffer
		this.source.loop = true;
		// Connect a filler process for audio buffer
		this.source.connect(this.filler);
		// Connect the source to the context's destination (the speakers)    
		this.filler.connect(this.context.destination);     
		// Start playing buffer
		this.source.noteOn(0); 
    },
    getRingBufferData: function(event) {
	    // Took buffer length bits for next loop
	    var output = event.outputBuffer.getChannelData(0);
	    // Check if enought byte to fill audio buffer
	    if (this.main.ringAvailableBytes >= output.length) {
		    // Fill audio buffer for next play
		    var curpos = 0;
		    for (var i = 0; i < output.length; i++)
		    {
			    // if end of ring buffer not reached
			    if (this.main.ringoffset+i<this.main.maxBufferSize) {
				    output[i] = this.main.ringbuffer[this.main.ringoffset+i];
				    curpos = i;
			    } else {
				    // else get the rest from pos 0 of buffer
				    output[i] = this.main.ringbuffer[i-curpos];				
			    }
		    }
		    // Adjust new offset in ring buffer
		    this.main.ringoffset += output.length;
		    if (this.main.ringoffset > this.main.maxBufferSize)
			    this.main.ringoffset = this.main.ringoffset - this.main.maxBufferSize;
		    // Decrease available byte read from buffer
		    this.main.ringAvailableBytes -= output.length;
	    }
    }
});	
