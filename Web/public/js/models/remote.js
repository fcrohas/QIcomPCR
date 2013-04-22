var RemoteControl = Backbone.Model.extend({
    defaults : {
      signal1 : 0,
      signal2 : 0,
      state   : "unknown",
      debug   : "",
      power   : false,
      frequency: 106500000,
      ifshift  : 128,
      modulation: "FM",
      data: "0"
    },
    initialize: function() { 
  
    },
    // Socket.IO management
    connect: function() {
	this.socket = io.connect();
	this.socket.model = this;
	this.socket.on('connect', this.onConnect);
	this.socket.on('message', this.onMessage);
	this.on('change:frequency', this.setFrequency);
    },
    onConnect: function() {
      //console.log(this.model);
      this.model.set('state', 'Connected');
      this.on('disconnect', this.model.onDisconnect);      
    },
    onDisconnect: function() {
      this.model.set('state', 'Disconnected');
    },
    onMessage: function(msg) {
      if (msg.substring(0,2) == 'SA') {
	this.model.set('signal1', msg.substring(2));
      }
      if (msg.substring(0,2) == 'SB') {
	this.model.set('signal2', msg.substring(2));
      }
      if (msg.substring(0,3) == 'DBG') {
	this.model.set('debug', msg.substring(3));
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
    },
    // Remote controller event
    togglePower: function() {
      if (this.get('power') == false) {
	this.socket.send('PWRON');
      } else {
	this.socket.send('PWROFF');
      }
      this.set('power', !this.get('power'));
    },
    setFrequency: function(model) {
      var value = model.get('frequency');
      if ((value != undefined) && (value !='')) {
	this.socket.send('FREQ'+value);
      }
    },
    setModulation: function(value) {
      this.socket.send('MOD'+value);
    },
    setIFShift: function(value) {
      this.socket.send('IF'+value);
    },
    setFilter: function(value) {
      console.log('FILTER'+value);
      this.socket.send('FILTER'+value);
    },
    setScope: function(value) {
      console.log('WT'+value);
      this.socket.send('WT1');
    }
});