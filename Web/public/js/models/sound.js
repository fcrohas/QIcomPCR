var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped'
    },
    initialize: function() { 
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
      console.log('meta : '+meta);
      codec = new Speex({
		    benchmark: false
	      , quality: 2
	      , complexity: 2
	      , bits_size: 15		  
      });
      //console.log('Speex initialized');	  
      stream.on('data', function(data) {
	      Speex.util.play(codec.decode(data));
	      //console.log('data received and played');	  
	      codec.close();
      });
  this.model.set('state', 'Playing');	
    },
    onDisconnect : function() {
      this.model.set('state', 'Stopped');	
    }
});	
