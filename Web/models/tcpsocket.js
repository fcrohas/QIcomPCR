 var net = require('net');

exports.TcpSocket = Backbone.Model.extend({
	defaults : {
		host : '127.0.0.1',
		port : '8888',
		connected : false
	},
	initialize : function() {
		this.connected = false;
		this.connect();
		// Next time try this
		// _.bindAll(this);

	},
	connect : function() {
		this.pcr = new net.Socket();
		this.pcr.model = this;
		this.pcr.on('error', this.onError);
		this.client = this.pcr.connect(this.get("port"), this.get("host"), this.onConnect);
		this.pcr.on("close", this.onClose);
	},
	onConnect :	function() {
		console.log('Connected to Pcr host '+this.remoteAddress+':'+this.remotePort);
		this.model.set("connected" , true);
	},
	onClose : function() {
	    console.log('Pcr connection closed');
	    this.model.set("connected" , false);
	},
	onError : function(e) {
		if ((e.code == 'ECONNRESET') || (e.code=='ECONNREFUSED')){
			console.log('Connection '+e.code+' , retrying...');
			var model = this.model;
			model.set("connected",false);
			model.client.end();
			setTimeout( function() {
				// Do not rebind with this one to onConnect
				model.client = model.pcr.connect(model.get("port"), model.get("host"));
			}, 1000);
		}
	}
});
