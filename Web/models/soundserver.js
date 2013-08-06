var binaryjs = require('binaryjs');

exports.SoundServer = Backbone.Model.extend({
	defaults : {
		path : '/stream'
	},
	initialize : function() {

	},
	start : function(server, soundpcr) {
		this.BinaryServer = binaryjs.BinaryServer;
		this.binarySocket = new this.BinaryServer({ server: server, path : this.get("path") });
		this.soundpcr = soundpcr;
		this.soundpcr.model = this;		
		this.binarySocket.model = this;
		this.binarySocket.on('connection', this.onConnect);		
	},
	onConnect :	function(client) {
	  console.log("Sound client connected");
	  this.model.client = client;
	  client.on('stream', this.model.onStream);
	  this.model.soundpcr.on('data', this.model.onData);
	},
	onStream : function(stream) {
	    console.log('client stream started!');
	},
	onData : function(data) {
		this.model.client.send(data);
	}

});
