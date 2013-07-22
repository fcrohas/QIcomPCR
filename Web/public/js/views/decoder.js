var DecoderView = Backbone.View.extend({
  initialize: function() {
    this.listenTo(this.model, "change:decodedText", this.setDecodedText);
    this.render();
  },
  render: function() {
    this.template = _.template($("#decoderTemplate").html(), this.model);
    this.$el.html(this.template);
    return this;
  },
  setDecodedText: function() {
	//console.log(this.model.get('decodedText'));
	if ( this.model.get('decodedText') != "" ) {
	    this.$el.find('#content').append(this.model.get('decodedText'));
	}
  }

});