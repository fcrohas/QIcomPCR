var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped'
    },
    initialize: function() { 
      this.codec = new Speex({
		    benchmark: false
	      , quality: 2
	      , complexity: 2
	      , bits_size: 15		  
      });
	  window.AudioContext = window.AudioContext || window.webkitAudioContext;
	  this.context = new AudioContext();
	  this.source = this.context.createBufferSource(); // creates a sound source	  
	  this.source.connect(this.context.destination);       // connect the source to the context's destination (the speakers)	  
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
    },
    onConnect: function() {
      this.on('disconnect', this.model.onDisconnect);      
    },
    onStream: function(stream,meta)  {
	  //var player = this.model.codec;
	  stream.source = this.model.source;
	  stream.context = this.model.context;
	  stream.codec  = this.model.codec;
      //console.log('meta : '+meta);
      //console.log('Speex initialized');	  
      stream.on('data', function(data) {
			//Speex.util.play(this.player.decode(data));
			this.source.buffer = this.context.decodeAudioData(this.codec.decode(data),function() {}, function(error) { console.log(error);});   // tell the source which sound to play
			this.source.start(0); 
			//console.log('data received and played');	  
      });
  this.model.set('state', 'Playing');	
    },
    onDisconnect : function() {
      this.model.set('state', 'Stopped');	
    }
});	
