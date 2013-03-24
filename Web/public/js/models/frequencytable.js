var FrequencyTable = Backbone.Model.extend({
  defaults : {
    step: [ 10, 50, 100, 500, 1000, 2000, 3000, 6000, 12500, 15000, 25000, 50000, 100000, 1000000, 10000000 ],
    position : 0,
    value : 500
  },
  initialize : function() {
    
  },
  increaseStep : function() {
    var position = this.get('position') + 1;
    var step = this.get('step');
    this.set('value', step[position]);
    this.set('position', position);
  },
  decreaseStep : function() {
    var position = this.get('position') - 1;
    var step = this.get('step');
    this.set('value', step[position]);
    this.set('position', position);
  }

});