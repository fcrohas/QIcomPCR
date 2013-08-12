var ControlsCmd = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:statecmd", this.setStatus);
    this.listenTo(this.model, "change:power", this.setPower);
    this.render();
  },
  render: function() {
    this.template = _.template($("#controlTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  events: {
      "click button.power": "togglePower",
	    "click button.nb": "toggleNB",
	    "click button.agc": "toggleAGC",
	    "click button.vsc": "toggleVSC"
  },  
  togglePower: function(event) {
    this.model.togglePower();
  },
  toggleAGC: function(event) {
    this.model.toggleAGC();
  },
  toggleVSC: function(event) {
    this.model.toggleVSC();
  },
  toggleNB: function(event) {
    this.model.toggleNB();
  },
  setStatus : function() {
  },
  setPower : function() {
    var power = this.model.get("power");
    if (power == true) {
      $(this.$el).find('.led-green').removeClass('off').addClass('on');
    } else {
      $(this.$el).find('.led-green').removeClass('on').addClass('off');
    }
  }
});
