var Display = Backbone.View.extend({
  initialize: function() {
    this.width = 640;
    this.height= 240;
    this.radio = this.model.get("radio");
    this.frequency1 = this.model.get("frequency1");
    this.frequency2 = this.model.get("frequency2");
    this.filter1 = this.model.get("filter1");
    this.filter2 = this.model.get("filter2");
    this.signal1 = this.model.get("signal1");
    this.signal2 = this.model.get("signal2");
    this.stepsize1 = freqTable.get('value');
    this.stepsize2 = freqTable.get('value');
    this.ifshift1= this.model.get("ifshift1");
    this.ifshift2= this.model.get("ifshift2");
    this.modulation1= this.model.get("modulation1");
    this.modulation2= this.model.get("modulation2");
    this.listenTo(this.model, "change:modulation1", this.setModulation);
    this.listenTo(this.model, "change:modulation2", this.setModulation);
    this.listenTo(this.model, "change:ifshift1", this.setIFShift);
    this.listenTo(this.model, "change:ifshift2", this.setIFShift);
    this.listenTo(this.model, "change:filter1", this.setFilter);
    this.listenTo(this.model, "change:filter2", this.setFilter);
    this.listenTo(this.model, "change:signal1", this.setSignal);
    this.listenTo(this.model, "change:signal2", this.setSignal);
    this.listenTo(this.model, "change:frequency1", this.setFrequency);
    this.listenTo(this.model, "change:frequency2", this.setFrequency);
    this.listenTo(this.model, "change:radio", this.setRadio);
    this.listenTo(this.model, "change:power", this.setpower);
    this.listenTo(freqTable, 'change:value',this.setStepSize);
    this.render();
  },
  //template: _.template($("#statusTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#displayTemplate").html(), this.model);
    this.$el.html(this.template);
    this.paper = Raphael("display", this.width, this.height); 
    this.paper.model = this.model;
    this.redraw(this.paper);
    return this;
  },
  events: {
    "DOMMouseScroll": "changeFrequency",
    "mousewheel":"changeFrequency",
    "contextmenu":"decreaseStep",
    "click":"increaseStep"
  },
  drawFrequency: function(paper) {
    var activate = "#0000FF";
    var deactivate = "#555555";
    this.freq1 = paper.text(this.width/4, this.height/3, this.frequency1).click(function() { paper.model.set("radio",1)});
    this.freq1.attr({fill:this.radio==1?activate:deactivate,stroke:"#FFFFFF",'font-size':"28"});
    this.freq2 = paper.text(this.width*3/4, this.height/3, this.frequency2).click(function() { paper.model.set("radio",2)});
    this.freq2.attr({fill:this.radio==2?activate:deactivate,stroke:"#FFFFFF",'font-size':"28"});
  },
  drawSignal: function(paper) {
    var grid = 4;
    var signal1 = this.signal1;
    // Draw scale 1
    var swidth = 252;
    var l1 = paper.path("M "+(this.width*0.25/grid)+" "+(this.height*1.33/3 + 17.0)+" l "+swidth+" 0 z");
    l1.attr({stroke:"#555555", 'stroke-width':5});
    for (var i=0; i < 255; i+=5) {
        // square
        var s = paper.rect((this.width*0.25/grid)+i,this.height*1.33/3, 2.0 +((i % 15) == 0 ?2.0:0.0), 11.0);
        if ( i < signal1)
	    s.attr("fill","#0000FF");
        else
	    s.attr("fill","#555555");
        // scale
        if ((i % 25) == 0 ) {
	    var height = 5;
	    var x = (this.width*0.25/grid)+i;
	    var y = (this.height*1.33/3) + 17.0;
	    var l = paper.path("M "+x+" "+y+" l 0 "+height+" z");
	    l.attr("stroke","#FF0000");
	    var t = paper.text((this.width*0.25/grid)+i,this.height*1.33/3 + 27.0, i);
	    t.attr("fill","#555555");
        }

    }    

    // Draw scale 2
    var signal2 = this.signal2;
    var l2 = paper.path("M "+(this.width*2.25/grid)+" "+(this.height*1.33/3 + 17.0)+" l "+swidth+" 0 z");
    l2.attr({stroke:"#555555", 'stroke-width':5});
    for (var i=0; i < 255; i+=5) {
        // square
        var s = paper.rect((this.width*2.25/grid)+i,this.height*1.33/3, 2.0 +((i % 15) == 0 ?2.0:0.0), 11.0);
        if ( i < signal2)
	    s.attr("fill","#0000FF");
        else
	    s.attr("fill","#555555");
        // scale
        if ((i % 25) == 0 ) {
	    var height = 5;
	    var x = (this.width*2.25/grid)+i;
	    var y = (this.height*1.33/3) + 17.0;
	    var l = paper.path("M "+x+" "+y+" l 0 "+height+" z");
	    l.attr("stroke","#FF0000");
	    var t = paper.text((this.width*2.25/grid)+i,this.height*1.33/3 + 27.0, i);
	    t.attr("fill","#555555");
        }

    }    
    
  },
  drawIF: function(paper) {
    var grid = 4;
    var IF1=this.ifshift1;
    var IF2=this.ifshift2;
    var filterwidth = 50;
    var axisx1 = (this.width*0.75/grid);
    var axisx2 = (this.width*0.25/grid);
    var axiswidth = axisx1 - axisx2;
    var axisy1 = this.height*2/3;
    var axisy2 = this.height*2.5/3 ;
    var axisheight = axisy1 -axisy2;
    var axismid = axisx1 + (axisx2 - axisx1)/2.0;
    var pos = IF1*axiswidth/255.0;

    var ax1 = paper.path("M "+axisx2+" "+axisy2+" l "+axiswidth+" 0 z");
    ax1.attr({stroke:"#AAAAAA"});
    var ay1 = paper.path("M "+axismid+" "+axisy2+" l 0 "+axisheight+" z");
    ay1.attr({ 'stroke-dasharray':"-",stroke:"#AAAAAA"});
    var band1 = paper.path("M "+(axisx2 + pos - (filterwidth*axiswidth/255.0) - (axiswidth/8.0))+" "+axisy2+" l "+(axiswidth/8.0)+" "+(axisheight * 4 /8)+" l "+(filterwidth*axiswidth/255.0)*2+" 0 l "+(axiswidth/8.0)+" "+(-1*axisheight * 4 /8));
    band1.attr({stroke:"#0000FF"});
    var label1 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label1.attr("fill","#9999FF");
    var t1 = paper.text(axismid,axisy2 +18.0, "IF");
    t1.attr({ fill:"#000000",'font-size':15});

    // Second filter
    axisx1 = (this.width*2.75/grid);
    axisx2 = (this.width*2.25/grid);
    axismid = axisx1 + (axisx2 - axisx1)/2.0;
    pos = IF2*axiswidth/255.0;

    var ax2 = paper.path("M "+axisx2+" "+axisy2+" l "+axiswidth+" 0 z");
    ax2.attr({stroke:"#AAAAAA"});
    var ay2 = paper.path("M "+axismid+" "+axisy2+" l 0 "+axisheight+" z");
    ay2.attr({ 'stroke-dasharray':"-",stroke:"#AAAAAA"});
    var band2 = paper.path("M "+(axisx2 + pos - (filterwidth*axiswidth/255.0) - (axiswidth/8.0))+" "+axisy2+" l "+(axiswidth/8.0)+" "+(axisheight * 4 /8)+" l "+(filterwidth*axiswidth/255.0)*2+" 0 l "+(axiswidth/8.0)+" "+(-1*axisheight * 4 /8));
    band2.attr({stroke:"#0000FF"});
    var label2 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label2.attr("fill","#9999FF");
    var t2 = paper.text(axismid,axisy2 +18.0, "IF");
    t2.attr({ fill:"#000000",'font-size':15});
  },
  drawFilter: function(paper) {
    var grid = 4;
    var Filter1=this.filter1;
    var Filter2=this.filter2;
    var filterwidth = 50;
    var axisx1 = (this.width*1.75/grid);
    var axisx2 = (this.width*1.25/grid);
    var axiswidth = axisx1 - axisx2;
    var axisy1 = this.height*2/3;
    var axisy2 = this.height*2.5/3 ;
    var axisheight = axisy1 -axisy2;
    var axismid = axisx1 + (axisx2 - axisx1)/2.0;
    var pos = 128*axiswidth/255.0;

    var ax1 = paper.path("M "+axisx2+" "+axisy2+" l "+axiswidth+" 0 z");
    ax1.attr({stroke:"#AAAAAA"});
    var ay1 = paper.path("M "+axismid+" "+axisy2+" l 0 "+axisheight+" z");
    ay1.attr({ 'stroke-dasharray':"-",stroke:"#AAAAAA"});
    var band1 = paper.path("M "+(axisx2 + pos - (Filter1*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter1*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
    band1.attr({stroke:"#0000FF"});
    var label1 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label1.attr("fill","#9999FF");
    var t1 = paper.text(axismid,axisy2 +18.0, Filter1/1000+"k");
    t1.attr({ fill:"#000000",'font-size':15});

    // Second filter
    axisx1 = (this.width*3.75/grid);
    axisx2 = (this.width*3.25/grid);
    axismid = axisx1 + (axisx2 - axisx1)/2.0;
    pos = 128*axiswidth/255.0;

    var ax2 = paper.path("M "+axisx2+" "+axisy2+" l "+axiswidth+" 0 z");
    ax2.attr({stroke:"#AAAAAA"});
    var ay2 = paper.path("M "+axismid+" "+axisy2+" l 0 "+axisheight+" z");
    ay2.attr({ 'stroke-dasharray':"-",stroke:"#AAAAAA"});
    var band2 = paper.path("M "+(axisx2 + pos - (Filter2*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter2*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
    band2.attr({stroke:"#0000FF"});
    var label2 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label2.attr("fill","#9999FF");
    var t2 = paper.text(axismid,axisy2 +18.0, Filter2/1000+"k");
    t2.attr({ fill:"#000000",'font-size':15});
  },
  drawStepSize: function(paper) {
    var left = this.width/4;
    var right = this.width*3/4;
    var top = this.height*2/3;
    var label1 = paper.rect(left - 25.0, top - 10.0, 50.0, 20.0);
    label1.attr("fill","#9999FF");
    var t1 = paper.text(left,top, this.stepsize1/1000+"k");
    t1.attr({ fill:"#000000",'font-size':15});

    var label2 = paper.rect(right - 25.0, top - 10.0, 50.0, 20.0);
    label2.attr("fill","#9999FF");
    var t2 = paper.text(right,top, this.stepsize2/1000+"k");
    t2.attr({ fill:"#000000",'font-size':15});
  },
  drawMode: function(paper) {
    var left = this.width/4;
    var right = this.width*3/4;
    var top = this.height*2.3/3;
    var StepSize = 12500;
    var label1 = paper.rect(left - 25.0, top - 10.0, 50.0, 20.0);
    label1.attr("fill","#9999FF");
    var t1 = paper.text(left,top, this.modulation1);
    t1.attr({ fill:"#000000",'font-size':15});

    var label2 = paper.rect(right - 25.0, top - 10.0, 50.0, 20.0);
    label2.attr("fill","#9999FF");
    var t2 = paper.text(right,top, this.modulation2);
    t2.attr({ fill:"#000000",'font-size':15});
  },
  redraw: function(paper) {
    // clear all
    paper.clear();
    var background = paper.rect( 0, 0, this.width, this.height);
    background.attr("fill","#000000");
    // draw frequencies
    this.drawFrequency(paper);
    // draw Signal
    this.drawSignal(paper);
    // draw IF
    this.drawIF(paper);
    // draw filter
    this.drawFilter(paper);
    // draw step size
    this.drawStepSize(paper);
    // draw mode
    this.drawMode(paper);
  },
  setFilter: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	this.filter1 = model.get("filter1");
    else if (radio == 2)
	this.filter2 = model.get("filter2");
    this.redraw(this.paper);
  },
  setSignal: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	this.signal1 = model.get("signal1");
    else if (radio == 2)
	this.signal2 = model.get("signal2");
    this.redraw(this.paper);
  },
  setIFShift: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	this.ifshift1 = model.get("ifshift1");
    else if (radio == 2)
	this.ifshift2 = model.get("ifshift2");
    this.redraw(this.paper);
  },
  setModulation: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	this.modulation1 = model.get("modulation1");
    else if (radio == 2)
	this.modulation2 = model.get("modulation2");
    this.redraw(this.paper);
  },
  setRadio: function(model) {
    this.radio = model.get("radio");
    this.redraw(this.paper);
  },
  setFrequency: function(model) {
    var radio = model.get("radio");
    if (radio ==1)
      this.frequency1 = model.get("frequency1");
    if (radio ==2)
      this.frequency2 = model.get("frequency2");
    this.redraw(this.paper);
  },
  changeFrequency: function(e) {
    // cross-browser wheel delta
    var e = window.event || e; // old IE support
    var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
    var frequency = this.model.get("frequency"+this.model.get("radio"));
    var freq = frequency + delta * freqTable.get('value');
    this.model.set("frequency"+this.model.get("radio"),freq);
  },
  decreaseStep : function (e) {
	freqTable.decreaseStep();
	return false;
  },
  increaseStep : function (e) {
	freqTable.increaseStep();
	return false;
  },
  setStepSize: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	this.stepsize1 = freqTable.get('value');
    else if (radio == 2)
	this.stepsize2 = freqTable.get('value');
    this.redraw(this.paper);
  }
});