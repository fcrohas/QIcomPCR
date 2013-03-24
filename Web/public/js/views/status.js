var StatusView = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:state", this.setStatus);
    this.render();
  },
  //template: _.template($("#statusTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#statusTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  setStatus: function(model) {
	if ( model.get("state") != "" ) {
	    $(this.$el).find('#statuscmd').html(model.get("state"));
	}
  }

});