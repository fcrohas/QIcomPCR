 var net = require('net');

exports.TcpSocket = Backbone.Model.extend({
	defaults : {
		host : '127.0.0.1',
		port : '8888'
	},
	initialize : function() {
		this.connected = false;
	},
	connect : function() {
		this.pcr = new net.Socket();
		this.pcr.model = this;
		this.pcr.connect(this.get("port"), this.get("host"), this.onConnect);
		this.pcr.on("close", this.onClose);
		return this.pcr;
	},
	onConnect :	function() {
		console.log('Connected to Pcr host '+this.remoteAddress+':'+this.remotePort);
		this.model.connected = true;
	},
	onClose : function() {
	    console.log('Pcr connection closed');
	    this.model.connected = false;
	}
});
