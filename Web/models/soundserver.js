var binaryjs = require('binaryjs');

exports.SoundServer = Backbone.Model.extend({
	defaults : {
		path : '/stream'
	},
	initialize : function() {
		this.connected = false;
	},
	start : function(server, soundpcr) {
		this.BinaryServer = binaryjs.BinaryServer;
		this.binarySocket = new this.BinaryServer({ server: server, path : this.get("path") });
		this.soundpcr = soundpcr.pcr;
		//this.soundpcr.model = this;		
		this.binarySocket.model = this;
		this.binarySocket.on('connection', this.onConnect);		
		this.binarySocket.on('error', this.onError);		
	},
	onConnect :	function(client) {
	  console.log("Sound client connected");
	  client.model = this.model;	
	  this.model.client = client;
	  client.on('error', this.model.onError);
	  client.on('stream', this.model.onStream);
	  client.on('close', this.model.onDisconnect);
	  var that = this;
	  this.model.soundpcr.on('data', function(data) { that.model.onData(data,that.model.client); });
	  client.model.connected = true;	  
	},
	onStream : function(stream) {
	    console.log('sound client stream started!');
	},
	onData : function(data,client) {
		if (client.model.connected == false)
			return;
		client.send(data);
	},
	onDisconnect : function() {
		this.model.connected = false;
		console.log("sound connection closed gracefully");
		this.close();
	},
	onError : function(e) {
		console.log("BinaryJS error code "+e.code);
	}

});
