var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped',
		maxBufferSize : 330750
    },
    initialize: function() { 
      // ******************  HTML5 VERSION *****************
      this.codec = new Speex({
	  benchmark: false
	  , quality: 8
	  , enh : 1
	  , complexity : 3
	  , bits_size: 38
	});
	//this.codec.set('SPEEX_SET_VBR',true);
	window.AudioContext = window.AudioContext || window.webkitAudioContext;
	this.context = new AudioContext();
	this.source = this.context.createBufferSource(0); // creates a sound source    
	this.source.connect(this.context.destination);       // connect the source to the context's destination (the speakers)    
	// Get buffer pointer to channel 0
	this.buffer = this.context.createBuffer(1, 110250, 22050); //  5s buffer			
	this.ringbuffer = new Float32Array(220500);
	this.ringsize = 0;
    },
    disconnect: function() {
	  codec.close();
    },
    // Socket.IO management
    connect: function() {
	  //this.socket = new BinaryClient('ws://'+$(location).attr('hostname')+':'+$(location).attr('port')+'/stream');
	  this.socket = new BinaryClient('ws://'+window.location.host+'/stream');
	  this.socket.model = this;
	  this.socket.on('open', this.onConnect);
	  this.socket.on('stream', this.onStream);
	  this.audio = null;
	  // ******************  APPLET VERSION *****************
	  // ****************** See SpeexAudio.jar **************
	  // Use SpeexAudio Applet
	  //var attributes = { id:'SpeexAudio', code:'com.lilisoft.Main.class',  width:100, height:50};
	  //var parameters = {jnlp_href: 'SpeexAudio.jnlp'} ;
	  //deployJava.runApplet(attributes, parameters, '1.6');		
	  //this.audio = SpeexAudio;
	  // ****************** See SpeexAudio.jar ************** 
	  
	  
    },
    onConnect: function() {
      this.on('disconnect', this.model.onDisconnect);      
    },
    onStream: function(stream,meta)  {
      // WebSocket stream received
      stream.audio = this.model.audio;
	  stream.model = this.model;
      stream.on('data', function(data) {
	    // ******************  HTML5 VERSION *****************
	    this.model.addToRingBuffer(data);
	    // ******************  APPLET VERSION *****************
	    // ****************** See SpeexAudio.jar **************
	    // Send received speex data to audio applet      
	    //this.audio.getAudio(data);
	    // ****************** See SpeexAudio.jar **************
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
	for (var i=0; i < decoded.length; i++) {
		this.ringbuffer[i + this.ringsize] = decoded[i];
	}
	//console.log(this.ringbuffer);		
	this.ringsize = this.ringsize + decoded.length +1;
	if (this.ringsize >= 110250) {
		this.playBuffer();
		// Copy datas over 110250
		for (var i=110250,j=0; i < this.ringsize; i++,j++) {
			this.ringsize[j] = this.ringsize[i];
			this.ringsize = j;							
		}
	}
	this.audioData = undefined;
    },
    playBuffer : function() {
	try {
		var buf = this.buffer.getChannelData(0);
		//console.log("Ring buffer size is " + this.ringsize);
		for (var i=0; i<110250;i++) {
			  buf[i] = this.ringbuffer[i];
		}
		this.source.buffer = this.buffer;   // tell the source which sound to play
		//console.log(this.source.buffer);	  
		this.source.loop = false;
		this.source.start(0); 
	} catch (e) {
		console.log(e.message);
	}
    }
});	
