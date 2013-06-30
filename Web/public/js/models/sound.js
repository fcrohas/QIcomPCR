var SoundControl = Backbone.Model.extend({
    defaults : {
		path : '/stream',
		state: 'Stopped'
    },
    initialize: function() { 
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
		// Use SpeexAudio Applet
		var attributes = { id:'SpeexAudio', code:'com.lilisoft.Main.class',  width:100, height:50};
		var parameters = {jnlp_href: 'SpeexAudio.jnlp'} ;
		deployJava.runApplet(attributes, parameters, '1.6');		
		this.audio = SpeexAudio;
		//this.audio = document.getElementById("SpeexAudio").getAudio();
		//this.audio.init();
    },
    onConnect: function() {
      this.on('disconnect', this.model.onDisconnect);      
    },
    onStream: function(stream,meta)  {
      // WebSocket stream received
      stream.audio = this.audio;
      stream.on('data', function(data) {
	  // Send received speex data to audio applet
	  this.audio.getAudio(data);
      });
  this.model.set('state', 'Playing');	
    },
    onDisconnect : function() {
      this.model.set('state', 'Stopped');	
    }
});	
