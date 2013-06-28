var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped'
    },
    initialize: function() { 
		// Use SpeexAudio Applet
		//var attributes = { id:'SpeexAudio',	code:'org.lilisoft/Main',  width:0, height:0};
		//var parameters = { } ;
		//deployJava.runApplet(attributes, parameters, '1.6');		
		//this.audio = SpeexAudio.getAudio();
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
	  // WebSocket stream received
	  stream.audio = this.audio;
      stream.on('data', function(data) {
			// Send received speex data to audio applet
			audio.setData(data);
      });
  this.model.set('state', 'Playing');	
    },
    onDisconnect : function() {
      this.model.set('state', 'Stopped');	
    }
});	
