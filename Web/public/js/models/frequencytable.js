var FrequencyTable = Backbone.Model.extend({
  defaults : {
    step: [ 10, 50, 100, 500, 1000, 2000, 3000, 6000, 12500, 15000, 25000, 50000, 100000, 1000000, 10000000 ],
    position : 0,
    value : 10
  },
  initialize : function() {
    this.on('change:position',this.setValue);
    
  },
  increaseStep : function() {
    var position = this.get('position') + 1;
    var step = this.get('step');
    // Block out of bound
    if (position == step.length)
	     position = step.length -1;
    this.set('position', position);
  },
  decreaseStep : function() {
    var position = this.get('position') - 1;
    // Block out of bound
    if (position < 0)
	     position = 0;
    this.set('position', position);
  },
  setValue: function(model) {
    var step = this.get('step');
    model.set('value', step[model.get("position")]);    
  },
  setPositionFromValue: function(value) {
    var step = this.get('step');
    if (step.indexOf(value) != -1)
	     this.set('position',step.indexOf(value));
  }

});