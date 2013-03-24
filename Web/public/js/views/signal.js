var SignalStrength = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:signal1", this.setSignal1);
    this.listenTo(this.model, "change:signal2", this.setSignal2);
    this.render();
  },
  //template: _.template($("#signalTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#signalTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  setSignal1: function() {
	var signal = this.model.get('signal1');
	if ( signal < 50 ) {
	    this.$el.find('#signal1').find('.bar-danger').width(signal*30/50);
	    this.$el.find('#signal1').find('.bar-warning').width('0%');
	    this.$el.find('#signal1').find('.bar-success').width('0%');
	}
	if (( signal > 90 ) && ( signal < 120 )) {
	    this.$el.find('#signal1').find('.bar-danger').width('30%');
	    this.$el.find('#signal1').find('.bar-warning').width((signal-90)*30/40);
	    this.$el.find('#signal1').find('.bar-success').width('0%');
	}
	if ( signal > 120 ) {
	    this.$el.find('#signal1').find('.bar-danger').width('30%');
	    this.$el.find('#signal1').find('.bar-warning').width('30%');
	    this.$el.find('#signal1').find('.bar-success').width((signal-120)*40/40);
	}
  },
  setSignal2: function() {
	var signal = this.model.get('signal2');
	if ( signal < 50 ) {
	    this.$el.find('#signal2').find('.bar-danger').width(signal*30/50);
	    this.$el.find('#signal2').find('.bar-warning').width('0%');
	    this.$el.find('#signal2').find('.bar-success').width('0%');
	}
	if (( signal > 90 ) && ( signal < 120 )) {
	    this.$el.find('#signal2').find('.bar-danger').width('30%');
	    this.$el.find('#signal2').find('.bar-warning').width((signal-90)*30/40);
	    this.$el.find('#signal2').find('.bar-success').width('0%');
	}
	if ( signal > 120 ) {
	    this.$el.find('#signal2').find('.bar-danger').width('30%');
	    this.$el.find('#signal2').find('.bar-warning').width('30%');
	    this.$el.find('#signal2').find('.bar-success').width((signal-120)*40/40);
	}
  }

});