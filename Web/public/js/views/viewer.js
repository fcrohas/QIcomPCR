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
    this.colorOffset = 50;
    $(this.el).find('.slider').slider().on('slide',this.changeColor);
    // Processing.Js
    this.p = new Processing(canvas[0], this.setupData);    
    this.p.view = this;
    this.p.setup = this.setup;
    this.p.draw = this.setData;
    this.p.setup();
    this.power = false;
    return this;
  },
  events: {
            "click button.start": "powertoggle",
	    "click ul li a": "refresh"
	    
  },
  getData: function()
  {
    //this.p.data = this.model.get("data"); 
    //console.log("receive data = "+this.p.data);
    
  },
  changeColor: function(e) {
    this.colorOffset = e.value;
  },
  setData: function()
  {
	var data = this.view.model.get("data");
	
	var length = parseInt(data.substring(0,4),16); // this is the length of array
	var pixel = "00";
	this.noStroke();
	this.colorMode(this.RGB, 100);
	if (this.line> this.height) {
		// Remove 5 lines length
		var lines = 2;
		for (var j=lines; j < this.height+1; j++)
		{
			for (var i=0; i< this.width; i++) 
			{
				var value = this.datas[j*this.width+i];
				if (value <= 20)
					this.stroke(0 , 0, 20+value);
				if ((value > 20) && (value < 40))
					this.stroke(60, value+20, 0);
				if (value >= 40)
					this.stroke(value+20, 0, 0);
				this.point(i, j );
				//store value before end
				this.datas[(j-lines)*this.width+i] = value;
			}
		}
		this.line = this.line - lines;
	}
	
	for(var i=0; i<length; i++) {
		var value = parseInt(data.substr(4+i*2,2),16);
		if (value <= 20)
			this.stroke(0 , 0, 20+value);
		if ((value > 20) && (value < 40))
			this.stroke(60, value+20, 0);
		if (value >= 40)
			this.stroke(value+20, 0, 0);
		this.point(i, this.line);
		//store value before end
		this.datas[this.line*this.width + i] = value;
	}
	this.line = this.line + 1;
  },
  powertoggle: function()
  {
    if (this.power == true) {
      this.model.setScope(false);
      this.p.background(0);
      this.p.noLoop();
      this.power = false;
    } else {
      this.model.setScope(true);
      this.power = true;
      this.p.loop();
    }
  },
  setupData: function (processing) {	
  },
  setup : function() {
	this.size(256, 200);
	this.frameRate(5);
	this.datas = new Array(this.height*this.width);
	//this.img = this.createImage(256,400,this.RGB);
	//this.img = this.loadImage("img/flipCounter-medium.png");
	this.background(0);
	this.line = 0;
  },
  refresh: function(e) {
    var element = $(e.target);
    var rate = element.text().replace(/[^0-9.]+/g,'');
    this.model.set("scopeRate",rate); // here it is milliseconds
    this.p.frameRate(1/rate); // Here it is frequency
    
  }
});