exports.SocketServer = Backbone.Model.extend({
	defaults : {
		path : '/command',
		allowScopeFrame : false
	},
	initialize : function() {
		this.connected = false;
	},
	start : function(socket, icompcr) {
		// Socket io
		this.socket = socket; 
		this.allowScopeFrame = this.get("allowScopeFrame");		
		// Hack for binary js to work
		this.socket.set("destroy upgrade",false);		
		this.icompcr = icompcr.pcr;
		// Set properties per path
		//this.icompcr.model = this;		
		// Scope frame is special case for heavyLoad
		this.path = this.socket.of(this.get("path"));
		this.path.model = this;
		this.path.icompcr = icompcr.pcr;
		this.path.on('connection', this.onConnect);
	},
	onConnect :	function(client) {
		console.log("client connected to "+this.model.get('path')+" allowScopeFrame:"+this.model.allowScopeFrame);
		client.allowScopeFrame = this.model.allowScopeFrame;	
		client.model = this.model;	
		client.icompcr = this.model.icompcr;
		// event for incomming message from Web Client
		client.on('message', this.model.onNode2Icom);
		client.on('disconnect', this.model.onDisconnect);
		// event for incomming message from device
		var that = this;
		this.model.icompcr.on('data', function(data) { that.model.onIcom2Node(data,client); });	  
		this.model.connected = true;		
	},
	onNode2Icom : function(msg) {
		console.log('client send message ('+msg+')');
		this.icompcr.write(msg);
		// Broadcast change to all user
		this.broadcast.emit('message',msg);
	},
	onIcom2Node : function(data,client) {
		// Discard if no more client connected
		if (client.model.connected == false)
			return;
	    // discard frame starting with WT
	    var msg = new String(data);
	    // try to find if multiple frame are here
	    var frame = msg.split('@');
	    for (var i=0; i < frame.length; i++) {
	    	if (frame[i] !='') {
		      	if (((frame[i].substring(0,2) != 'WT') && (frame[i].substring(0,3) != 'BDS') && (client.allowScopeFrame == false)) || ((client.allowScopeFrame == true) && ((frame[i].substring(0,2) == 'WT') || (frame[i].substring(0,3) == 'BDS')))) {
		      	  client.volatile.send(frame[i]);
		      	}
	    	}
    	}
    },
    onDisconnect : function() {
    	this.model.connected = false;
    	console.log('client disconnected');
    }
});

