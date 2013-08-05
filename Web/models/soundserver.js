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
		this.binarySocket.on('connection', this.onConnect);
		this.soundpcr = soundpcr;		
		this.binarySocket.model = this;
	},
	onConnect :	function(client) {
	  client.on('stream', this.model.onStream);
	  console.log("Sound client connected");
	  client.soundpcr.on('data', client.onData);
	},
	onStream : function(stream) {
	    console.log('client stream started!');
	},
	onData : function(data) {
		this.model.client.send(data);
	}

});
