var ViewerView = Backbone.View.extend({
  initialize: function() {
    //this.listenTo(this.model, "change:data", this.getData);
    this.render();
  },
  //template: _.template($("#signalTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#viewerTemplate").html(), this.model);
    this.$el.html(this.template);
    var canvas = $(this.$el).find("canvas");
    // Processing.Js
    this.p = new Processing(canvas[0], this.setupData);    
    this.p.view = this;
    this.p.setup = this.setup;
    this.p.draw = this.setData;
    this.p.setup();
    return this;
  },
  events: {
            "click button": "toggleScope"
  },
  getData: function()
  {
    //this.p.data = this.model.get("data"); 
    //console.log("receive data = "+this.p.data);
    
  },
  setData: function()
  {
    //var data = this.p.view.model.get("data"); 
    var data = this.view.model.get("data");
    //console.log(this.view.model.get("data"));
    var length = parseInt(data.substring(0,4),16); // this is the length of array
    //console.log(data);
    // Start to read array to display value
    // read first pixel
    var pixel = "00";
    this.loadPixels();
    this.line = 1;
    var pixels = this.pixels.toArray();
    for( var i=1; i < length; i++) {
	// Get next pixels
	//this.set(this.line, i, this.color(240,128,parseInt(data.substr(4+i*2,2),16)));
	//this.background(parseInt(data.substr(4+i*2,2),16));
	this.pixels.setPixel(i+this.line * length, 13456); //[i+this.line * length] = 123456; //this.color(240,128,parseInt(data.substr(4+i*2,2),16));
	//this.img.pixels[i] = new this.color(0, 90, 102); 
	// Increment line
	this.line = this.line + 1;
    }
    //this.pixels.set(pixels);
    console.log(this.pixels.toArray());
    this.updatePixels();
    //this.image(this.img,0,0);
  },
  toggleScope: function()
  {
    this.model.setScope(true);
  },
  setupData: function (processing) {	
  },
  setup : function() {
	this.size(256, 400);
	this.frameRate(1);
	//this.img = this.createImage(256,400,this.RGB);
	//this.img = this.loadImage("img/flipCounter-medium.png");
	this.background(255);
  }
});