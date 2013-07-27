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
  events: {
            "click #decoder ul li a": "setDecoder",
	    "click #channel ul li a": "setChannel"
  },
  setDecodedText: function() {
	//console.log(this.model.get('decodedText'));
	if ( this.model.get('decodedText') != "" ) {
	    this.$el.find('#content').append(this.model.get('decodedText'));
	}
  },
  setChannel: function(e) {
    var element = $(e.target);
    var channel = parseInt(element.text(),10);
    this.model.set("channel",channel); // here it is milliseconds    
  },
  setDecoder: function(e) {
    var element = $(e.target);
    var name = element.text();
    this.model.set("decoder",name); // here it is milliseconds    
  }
  

});