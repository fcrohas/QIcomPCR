var RemoteControl = Backbone.Model.extend({
    defaults : {
      commandPath :'/command',
      dataPath :'/data',
      signal1 : 0,
      signal2 : 0,
      statecmd: "unknown",
      statedata: "unknown",
      selectedFrequency: -1,
      selectedBandwidth: 120, // around 130 hz
      debug : "",
      decodedText : "",
      power : false,
      radio : 1,
      frequency1: 106500000,
      ifshift1  : 128,
      modulation1: "WFM",
      filter1: 230000,
      frequency2: 145500000,
      ifshift2  : 128,
      modulation2: "FM",
      filter2: 15000,
      data: "0",
      scopeRate: "1",
      nb: false,
      agc: false,
      vsc: false,
      decoder: "none",
      channel:0,
      changeAllowed: true
    },
    initialize: function() { 
      this.skipUpdate = false;
      this.ack = true;
    },
    // Socket.IO management
    connect: function() {
    	this.cmd = io.connect('http://'+window.location.host+this.get('commandPath'));
    	this.data = io.connect('http://'+window.location.host+this.get('dataPath'));
    	this.cmd.model = this;
    	this.cmd.on('connect', this.onConnectCmd);
    	this.cmd.on('message', this.onMessage);
    	this.data.model = this;
    	this.data.on('connect', this.onConnectData);
    	this.data.on('message', this.onData);
    	this.on('change:frequency1', this.setFrequency);
    	this.on('change:frequency2', this.setFrequency);
    	this.on('change:radio', this.setRadio);
    	this.on('change:scopeRate', this.setScopeRate);
    	this.on('change:selectedFrequency', this.setSelectedFrequency);
      this.on('change:selectedBandwidth', this.setSelectedBandwidth);
    	this.on('change:decoder', this.setDecoder);
    	this.on('change:channel', this.setChannel);
    },
    onConnectCmd: function() {
      //console.log(this.model);
      this.model.set('statecmd', 'Connected');
      this.on('disconnect', this.model.onDisconnect);      
    },
    onConnectData: function() {
      //console.log(this.model);
      this.model.set('statedata', 'Connected');
      this.on('disconnect', this.model.onDisconnect);      
    },
    onDisconnectCmd: function() {
      this.model.set('statecmd', 'Disconnected');
    },
    onDisconnectData: function() {
      this.model.set('statedata', 'Disconnected');
    },
    onMessage: function(msg) {
      // block until acknowledge is received
      //console.log("message="+msg+" ack="+this.model.ack+" changeAllowed="+this.model.get("changeAllowed"));
      if (this.model.ack == false) {
        if (msg == "ACK") {
            // Acknowledge received reinit it
            this.model.ack = true;
            this.model.set("changeAllowed", true);
        }
        return;
      }
      // block command send until it come from outside
      if (msg.substring(0,2) == 'SA') {
      	if (Math.abs(this.model.get("signal1")-parseInt(msg.substring(2),10)) > 10) {
      	  this.model.set('signal1', msg.substring(2));
      	}
      }
      if (msg.substring(0,2) == 'SB') {
      	if (Math.abs(this.model.get("signal2")-parseInt(msg.substring(2),10)) > 10) {
      	  this.model.set('signal2', msg.substring(2));
      	}
      }
      if (msg.substring(0,3) == 'DBG') {
      	this.model.set('debug', msg.substring(3));
      }
      if (msg.substring(0,4) == 'DEM\t') {
      	this.model.set('decodedText', msg.substring(4));
      }
      if (msg.substring(0,6) == 'PWROFF') {
      	this.model.set('power', false);
      }
      if (msg.substring(0,5) == 'PWRON') {
      	this.model.set('power', true);
      }
      if (msg.substring(0,2) == 'WT') {
      	this.model.set('data', msg.substring(2));
      }
      if (msg.substring(0,4) == 'FREQ') {
        // This message are for other component not this one
        this.model.set("changeAllowed", false);
        this.model.set('frequency'+this.model.get('radio'),new Number(msg.substring(4)));
      }
      if (msg.substring(0,5) == 'RADIO') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set('radio',new Number(msg.substring(5))+1);
      }
      if (msg.substring(0,3) == 'MOD') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("modulation"+this.model.get("radio"),msg.substring(3));
      }
      if (msg.substring(0,2) == 'IF') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("ifshift"+this.model.get("radio"),msg.substring(2));
      }
      if (msg.substring(0,6) == 'FILTER') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("filter"+this.model.get("radio"),msg.substring(6));
      }
      if (msg.substring(0,3) == 'AGC') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("agc",(msg.substring(3)=="ON")? true: false);
      }
      if (msg.substring(0,3) == 'VSC') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("vsc",(msg.substring(3)=="ON")? true: false);
      }
      if (msg.substring(0,2) == 'NB') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("nb",(msg.substring(2)=="ON")? true: false);
      }
      if (msg.substring(0,5) == 'SFREQ') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("selectedFrequency",new Number(msg.substring(5)));
      }
      if (msg.substring(0,5) == 'SFRQW') {
        // This message are for other component
        this.model.set("changeAllowed", false);
        this.model.set("selectedBandwidth",new Number(msg.substring(5)));
      }
    },
    onData: function(msg) {
      if (msg.substring(0,2) == 'WT') {
	       this.model.set('data', msg.substring(2));
      }
    },
    // Remote controller event
    togglePower: function() {
      if (this.get('power') == false) {
		    this.cmd.send('PWRON');
      } else {
		    this.cmd.send('PWROFF');
      }
      this.set('power', !this.get('power'));
    },
    setFrequency: function(model) {
      if (model.get("changeAllowed") == false)
          return;
      var value = model.get('frequency'+model.get('radio'));
      if ((value != undefined) && (value !='')) {
		    this.cmd.send('FREQ'+value);
      }
    },
    setSelectedFrequency: function(model) {
      if (model.get("changeAllowed") == false)
          return;
      var value = model.get('selectedFrequency');
      this.cmd.send('SFREQ'+value);
    },
    setSelectedBandwidth: function(model) {
      if (model.get("changeAllowed") == false)
          return;
      var value = model.get('selectedBandwidth');
      this.cmd.send('SFRQW'+value);
    },
    setModulation: function(value) {
      this.set("modulation"+this.get("radio"),value);
      this.cmd.send('MOD'+value);
    },
    setIFShift: function(value) {
      this.set("ifshift"+this.get("radio"),value);
      this.cmd.send('IF'+value);
    },
    setFilter: function(value) {
      this.set("filter"+this.get("radio"),value);
      this.cmd.send('FILTER'+value);
    },
    setScope: function(value) {
      if (value == true)
		    this.cmd.send('WT'+this.get("scopeRate"));
      else
		    this.cmd.send('WTOFF');
    },
    setScopeRate: function(model) {
		  this.cmd.send('WT'+model.get("scopeRate"));
    },
    setRadio: function(model) {
      if (model.get("changeAllowed") == false)
          return;
      this.cmd.send('RADIO'+(model.get("radio")-1));
    },
    toggleAGC: function() {
      if (this.get('agc') == false) {
		    this.cmd.send('AGCON');
      } else {
		    this.cmd.send('AGCOFF');
      }
      this.set('agc', !this.get('agc'));
    },
    toggleNB: function() {
      if (this.get('nb') == false) {
		    this.cmd.send('NBON');
      } else {
		    this.cmd.send('NBOFF');
      }
      this.set('nb', !this.get('nb'));
    },
    toggleVSC: function() {
      if (this.get('vsc') == false) {
		    this.cmd.send('VSCON');
      } else {
		    this.cmd.send('VSCOFF');
      }
      this.set('vsc', !this.get('vsc'));
    },
    setChannel: function(model) {
      this.cmd.send('CHAN'+model.get("channel"));
    },
    setDecoder: function(model) {
      var decoder = model.get("decoder");
      var pos = 0;
      if (decoder == "None") pos=0;
      if (decoder == "Acars") pos=1;					  
      if (decoder == "Acars GPL") pos=2;
      if (decoder == "CW") pos=3;
      if (decoder == "RTTY") pos=4;					  					  
      this.cmd.send('DEC'+pos);      
    }
});