var BandScope = Backbone.View.extend({
  initialize: function() {
    this.width = 640;
    this.height= 200;
    this.size = 1000000;
    this.step = 100000;
    this.state=false;
    this.widthTable = ['5Mhz','2Mhz','1Mhz','500Khz','200Khz','100Khz','50Khz'];
    this.stepTable = ['100Khz','50Khz','25Khz','15Khz','12.5Khz','10Khz','6Khz','5Khz','2.5Khz','1Khz'];
    this.listenTo(this.model,'change:bandscope', this.setData);
    this.listenTo(this.model,'change:frequency1', this.setCentralFrequency);
    this.samples = new Array(256); 
    this.min = 128 - (this.size / this.step) / 2;
    this.max = 128 + (this.size / this.step) / 2;
    //console.log("min="+this.min+" max="+this.max);
    this.render();
  },
  render: function() {
    this.template = _.template($("#bandscopeTemplate").html(), this.model);
    this.$el.html(this.template);
    this.paper = Raphael($(this.$el).find(".bandscopeDisplay").get(0), this.width, this.height); 
    this.paper.model = this.model;
    this.redraw(this.paper);
    return this;
  },
  events: {
  		"click .start": "startBandscope",
    	"click .bandwidth ul li a": "setBandWidth",
	    "click .step ul li a": "setStep"
  },
  setData: function(model)
  {
  	var data = model.get('bandscope');
  	var offset = parseInt(data.substring(0,2),16); // get packet offset
  	//console.log("sample offset="+offset);
    for(var i=0; i < 16; i++) {
        // update sample at position with new value
        try {
	        var sample = data.substr(2+(i*2),2);
	        var height = parseInt(sample,16) * this.height / 255;
	        // draw samples
	        this.samples[offset+i].attr('y',this.height - height);
	        this.samples[offset+i].attr('height',height);
	    } catch(e) {

	    }
    }  	
  }, 
  redraw : function(paper) {
    paper.clear();
    var background = paper.rect( 0, 10, this.width, this.height);
    background.attr("fill","#000000");
    // Build graph
    var xAxis = this.width/(this.size/this.step);
    //console.log("step="+(this.size/this.step)+" xAxis="+xAxis);
    for(var i=0; i<this.size/this.step; i++) {
    	//console.log("Axis start at "+(this.min+i));
    	this.samples[this.min+i] = paper.rect(i*xAxis,this.height-5,xAxis,5);
    	this.samples[this.min+i].attr("fill","#0000ff");
    }
  },
  setBandWidth : function(e)
  {
  	this.size = $(e.target).text().replace("Mhz","000000");
  	this.size = this.size.replace("Khz","000");
  	this.size = this.size.replace(".","") * 1;

  	//console.log('Bandwidth='+this.size);
    // Adjust scale to new bandwidth
    // 80 hex is middle so
    this.min = 128 - (this.size / this.step) / 2;
    this.max = 128 + (this.size / this.step) / 2;
    this.model.set("bandscopeWidth",this.widthTable.indexOf($(e.target).text()));
    this.redraw(this.paper);
  },
  setStep : function(e)
  {
  	this.step = $(e.target).text().replace("Mhz","000000");
  	this.step = this.step.replace("Khz","000");
  	this.step = this.step.replace(".","") * 1;

    //console.log('BandStep='+this.step);
    this.min = 128 - (this.size / this.step) / 2;
    this.max = 128 + (this.size / this.step) / 2;
    this.model.set("bandscopeStep",this.stepTable.indexOf($(e.target).text()));
    this.redraw(this.paper);
  },
  setCentralFrequency : function(model)
  {
    this.frequency = model.get('frequency1');
    this.min = 128 - (this.size / this.step) / 2;
    this.max = 128 + (this.size / this.step) / 2;
  },
  startBandscope : function(e) {
  	this.state=!this.state;
  	this.model.set("bandscopeState",this.state);
  }
});