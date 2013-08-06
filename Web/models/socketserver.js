exports.SocketServer = Backbone.Model.extend({
	defaults : {
		path : '/command',
		allowScopeFrame : false
	},
	initialize : function() {
	},
	start : function(socket, icompcr) {
		// Socket io
		this.socket = socket; 
		this.allowScopeFrame = this.get("allowScopeFrame");		
		// Hack for binary js to work
		this.socket.set("destroy upgrade",false);		
		this.icompcr = icompcr;
		// Set properties per path
		this.icompcr.model = this;		
		// Scope frame is special case for heavyLoad
		this.path = this.socket.of(this.get("path"));
		this.path.model = this;
		this.path.icompcr = icompcr;
		this.path.on('connection', this.onConnect);
	},
	onConnect :	function(client) {
		console.log("client connected to "+this.model.get('path')+" allowScopeFrame:"+this.model.allowScopeFrame);
		this.model.client = client;
		client.icompcr = this.model.icompcr;
		client.allowScopeFrame = this.model.allowScopeFrame;		
		// event for incomming message from Web Client
		client.on('message', this.model.onNode2Icom);
		client.on('disconnect', this.model.onDisconnect);
		// event for incomming message from device
		that = this;
		client.icompcr.on('data', function(data) { that.model.onIcom2Node(data); });	  
	},
	onNode2Icom : function(msg) {
		console.log('client send message ('+msg+')');
		this.icompcr.write(msg);
	},
	onIcom2Node : function(data) {
	    // discard frame starting with WT
	    var msg = new String(data);
	    // try to find if multiple frame are here
	    var frame = msg.split('@');
	    for (var i=0; i < frame.length; i++) {
	    	if (frame[i] !='') {
		      	if ((frame[i].substring(0,2) != 'WT') || (this.allowScopeFrame == true) ) {
		      	  this.client.volatile.send(frame[i]);
		      	}
	    	}
    	}
    },
    onDisconnect : function() {
    	console.log('client disconnected');
    }
});

