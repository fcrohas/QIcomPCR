var FrequencyStepView = Backbone.View.extend({
  initialize: function() {
    this.listenTo(freqTable, 'change:value',this.refresh);    
    this.render();
  },
  events: {
            "DOMMouseScroll": "MouseWheelHandler",
	    "mousewheel":"MouseWheelHandler",
	    "contextmenu":"decreaseStep",
	    "click":"increaseStep"
  },
  render: function() {
    this.template = _.template($("#frequencyTemplate").html(), this.model);
    this.$el.html(this.template);
    $(this.$el).find('#digits').flipCounter({
        number:freqTable.get('value'), // the initial number the counter should display, overrides the hidden field
        numIntegralDigits:10, // number of places left of the decimal point to maintain
        numFractionalDigits:0, // number of places right of the decimal point to maintain
        digitClass:"counter-digit", // class of the counter digits
        counterFieldName:"counter-value", // name of the hidden field
        digitHeight:40, // the height of each digit in the flipCounter-medium.png sprite image
        digitWidth:30, // the width of each digit in the flipCounter-medium.png sprite image
        imagePath:"img/flipCounter-medium.png", // the path to the sprite image relative to your html document
        easing: false, // the easing function to apply to animations, you can override this with a jQuery.easing method
        duration:10000, // duration of animations
        onAnimationStarted:false, // call back for animation upon starting
        onAnimationStopped:false, // call back for animation upon stopping
        onAnimationPaused:false, // call back for animation upon pausing
        onAnimationResumed:false, // call back for animation upon resuming from pause
	formatNumberOptions:{format:"00,000,000",locale:"fr,en"}
    });    
    return this;
  },
  MouseWheelHandler : function (e) {
	// cross-browser wheel delta
	var e = window.event || e; // old IE support
	var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
	if (delta > 0 ) freqTable.increaseStep();
        if (delta < 0 ) freqTable.decreaseStep();
        $(this.$el).find('#digits').flipCounter("setNumber",freqTable.get('value'));
  },
  decreaseStep : function (e) {
	freqTable.decreaseStep();
	$(this.$el).find('#digits').flipCounter("setNumber",freqTable.get('value'));
	return false;
  },
  increaseStep : function (e) {
	freqTable.increaseStep();
	$(this.$el).find('#digits').flipCounter("setNumber",freqTable.get('value'));
  },
  refresh : function() {
	$(this.$el).find('#digits').flipCounter("setNumber",freqTable.get('value'));
  }
});