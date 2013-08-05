var DebugView = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:debug", this.setDebugText);
    this.render();
  },
  //template: _.template($("#debugTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#debugTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  setDebugText: function() {
	console.log(this.model.get('debug'));
	if ( this.model.get('debug') != "" ) {
	    this.$el.find('.scrollbox').append(this.model.get('debug')+'<br>');
	}
  }

}); 