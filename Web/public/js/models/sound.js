var SoundControl = Backbone.Model.extend({
    defaults : {
	path : '/stream',
	state: 'Stopped',
	maxBufferSize : 110250,
	isApplet : false,
	resample : false
	
    },
    initialize: function() { 
      // ******************  HTML5 VERSION *****************
    this.codec = new Speex({
	benchmark: false
	, quality: 4
	, enh : 1
	, complexity : 3
	, bits_size: 24
      });
      window.AudioContext = window.AudioContext || window.webkitAudioContext;
      if ( (window.AudioContext == null) || (window.AudioContext == undefined)) {
	this.set('isApplet',false);
      }
      this.maxBufferSize = this.get('maxBufferSize');
      if (this.get('isApplet') == false) {
	this.context = new AudioContext();
	this.source = this.context.createBufferSource(0); // creates a sound source    
	this.source.connect(this.context.destination);       // connect the source to the context's destination (the speakers)    
	// Get buffer pointer to channel 0
	this.buffer = this.context.createBuffer(1, this.maxBufferSize, 22050); //  5s buffer	
	if (this.get('resample') == true) {
	  this.resampleControl = new Resampler(11025,22050,1,512,true);
	}
	// for ring buffer allocate twice the size
	this.ringbuffer = new Float32Array(this.maxBufferSize*2); 
	this.ringsize = 0;
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
	} else {
	  for (var i=0; i < decoded.length; i++) {
	      this.ringbuffer[i + this.ringsize] = decoded[i];
	  }
	}
	//console.log(this.ringbuffer);		
	this.ringsize = this.ringsize + decoded.length +1;
	if (this.ringsize >= this.maxBufferSize) {
		this.playBuffer();
		// Copy datas over 110250
		for (var i=this.maxBufferSize,j=0; i < this.ringsize; i++,j++) {
			this.ringsize[j] = this.ringsize[i];
			this.ringsize = j;							
		}
	}
	this.audioData = undefined;
    },
    playBuffer : function() {
	//try {
		console.log("Play Buffer");
		var buf = this.buffer.getChannelData(0);
		//console.log("Ring buffer size is " + this.ringsize);
		for (var i=0; i<this.maxBufferSize;i++) {
			  buf[i] = this.ringbuffer[i];
		}
		this.source.buffer = this.buffer;   // tell the source which sound to play
		//console.log(this.source.buffer);	  
		this.source.loop = false;
		this.source.start(0); 
	/*} catch (e) {
		console.log(e.message);
	}*/
    }
});	
