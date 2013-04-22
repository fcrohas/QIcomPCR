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
				if (value < 85)
					this.stroke(0 , 0, value);
				if ((value > 85) && (value < 170))
					this.stroke(255, value, 0);
				if (value > 170)
					this.stroke(value, 0, 0);
				this.point(i, j );
				//store value before end
				this.datas[(j-lines)*this.width+i] = value;
			}
		}
		this.line = this.line - lines;
	}
	
	for(var i=0; i<length; i++) {
		var value = parseInt(data.substr(4+i*2,2),16);
		if (value < 85)
			this.stroke(0 , 0, value);
		if ((value > 85) && (value < 170))
			this.stroke(255, value, 0);
		if (value > 170)
			this.stroke(value, 0, 0);
		this.point(i, this.line);
		//store value before end
		this.datas[this.line*this.width + i] = value;
	}
	this.line = this.line + 1;
  },
  toggleScope: function()
  {
    this.model.setScope(true);
  },
  setupData: function (processing) {	
  },
  setup : function() {
	this.size(256, 200);
	this.frameRate(50);
	this.datas = new Array(this.height*this.width);
	//this.img = this.createImage(256,400,this.RGB);
	//this.img = this.loadImage("img/flipCounter-medium.png");
	this.background(255);
	this.line = 0;
  }
});