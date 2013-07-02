var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped'
    },
    initialize: function() { 
      // ******************  HTML5 VERSION *****************
      this.codec = new Speex({
	  benchmark: false
	  , mode : 0
	  , enh : 1
	  , frame_size : 160
	  , quality: 8
	  , bits_size: 38    
	});
	window.AudioContext = window.AudioContext || window.webkitAudioContext;
	this.context = new AudioContext();
	this.source = this.context.createBufferSource(0); // creates a sound source    
	this.source.connect(this.context.destination);       // connect the source to the context's destination (the speakers)    
	this.buffer = this.context.createBuffer(1, 4096, 22050);
      // *******************************************************
    },
    disconnect: function() {
	  codec.close();
    },
    // Socket.IO management
    connect: function() {
	  this.socket = new BinaryClient('ws://'+$(location).attr('hostname')+':'+$(location).attr('port')+'/stream');
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
      stream.audio = this.audio;
      stream.source = this.model.source;
      stream.context = this.model.context;
      stream.codec  = this.model.codec; 
      stream.buffer = this.model.buffer;
      stream.on('data', function(data) {
	  // ******************  HTML5 VERSION *****************
	  // Get buffer pointer to channel 0
	  var buf = this.buffer.getChannelData(0);
	  // Convert ArrayBuffer to Int8Array
	  var audioData = new Int8Array(data);
	  // Decode Speex buffer
	  var decoded = this.codec.decode(audioData);
	  // Copie it to source buffer
	  console.log(decoded.length);
	  for (var i=0; i<decoded.length;i++) {
	    buf[i] = decoded[i];
	  }
	  this.source.buffer = this.buffer;   // tell the source which sound to play
	  console.log(this.source.buffer);	  
	  this.source.noteOn(0);//this.context.currentTime); 	
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
    }
});	
