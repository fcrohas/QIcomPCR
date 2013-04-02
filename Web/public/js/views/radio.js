var RadioCmd = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:state", this.setStatus);
    this.listenTo(this.model, "change:power", this.setpower);
    this.render();
  },
  //template: _.template($("#statusTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#radioTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  events: {
            "click button.power": "togglePower",
	    "click button.wfm": "toggleWFM",
	    "click button.fm": "toggleFM",
	    "click button.am": "toggleAM",
	    "click button.lsb": "toggleLSB",
	    "click button.usb": "toggleUSB",
	    "click button.cw": "toggleCW",
	    "click button.230k": "toggle230k",
	    "click button.50k": "toggle50k",
	    "click button.15k": "toggle15k",
	    "click button.6k": "toggle6k",
	    "click button.28k": "toggle28k"
  },  
  togglePower: function(event) {
    this.model.togglePower();
  },
  toggleWFM: function(event) {
    this.model.setModulation('WFM');
  },
  toggleFM: function(event) {
    this.model.setModulation('FM');
  },
  toggleAM: function(event) {
    this.model.setModulation('AM');
  },
  toggleLSB: function(event) {
    this.model.setModulation('LSB');
  },
  toggleUSB: function(event) {
    this.model.setModulation('USB');
  },
  toggleCW: function(event) {
    this.model.setModulation('CW');
  },
  toggle230k: function(event) {
    this.model.setFilter('230K');
  },
  toggle50k: function(event) {
    this.model.setFilter('50K');
  },
  toggle15k: function(event) {
    this.model.setFilter('15K');
  },
  toggle6k: function(event) {
    this.model.setFilter('6K');
  },
  toggle28k: function(event) {
    this.model.setFilter('28K');
  },
  setStatus : function() {
  },
  setPower : function() {
  }
});