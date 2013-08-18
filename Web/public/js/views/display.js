var Display = Backbone.View.extend({
  initialize: function(options) {
    this.width = 640;
    this.height= 240;
    this.freqTable = options.frequencytable;
    this.skipChange = false;
    this.radio = this.model.get("radio");
    this.frequency1 = this.model.get("frequency1");
    this.frequency2 = this.model.get("frequency2");
    this.filter1 = this.model.get("filter1");
    this.filter2 = this.model.get("filter2");
    this.signal1 = this.model.get("signal1");
    this.signal2 = this.model.get("signal2");
    this.stepsize1 = this.freqTable.get('value');
    this.stepsize2 = this.freqTable.get('value');
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
    this.listenTo(this.model, "change:nb", this.setnb);
    this.listenTo(this.model, "change:agc", this.setagc);
    this.listenTo(this.model, "change:vsc", this.setvsc);
    this.listenTo(this.model, "change:changeAllowed", this.onChangeAllowed);
    this.listenTo(this.freqTable, "change:value",this.setStepSize);
    this.render();
  },
  //template: _.template($("#statusTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#displayTemplate").html(), this.model);
    this.$el.html(this.template);
    this.paper = Raphael("display", this.width, this.height); 
    this.paper.model = this.model;
    this.draw(this.paper);
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
    this.freq1 = paper.text(this.width/4, this.height/3, this.frequency1).click(function(e) { paper.model.set("radio",1); e.stopImmediatePropagation(); return false;});
    this.freq1.attr({fill:this.radio==1?activate:deactivate,stroke:"#FFFFFF",'font-size':"28"});
    this.freq2 = paper.text(this.width*3/4, this.height/3, this.frequency2).click(function(e) { paper.model.set("radio",2); e.stopImmediatePropagation(); return false;});
    this.freq2.attr({fill:this.radio==2?activate:deactivate,stroke:"#FFFFFF",'font-size':"28"});
  },
  drawSignal: function(paper) {
    var grid = 4;
    var signal1 = this.signal1;
    // Draw scale 1
    var swidth = 252;
    var l1 = paper.path("M "+(this.width*0.25/grid)+" "+(this.height*1.33/3 + 17.0)+" l "+swidth+" 0 z");
    l1.attr({stroke:"#555555", 'stroke-width':5});
    this.signalStrength1 = new Array(255);
    for (var i=0; i < 255; i+=5) {
        // square
        this.signalStrength1[i] = paper.rect((this.width*0.25/grid)+i,this.height*1.33/3, 2.0 +((i % 15) == 0 ?2.0:0.0), 11.0);
        if ( i < signal1)
	       this.signalStrength1[i].attr("fill","#0000FF");
        else
	       this.signalStrength1[i].attr("fill","#555555");
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
    this.signalStrength2 = new Array(255);
    for (var i=0; i < 255; i+=5) {
        // square
        this.signalStrength2[i] = paper.rect((this.width*2.25/grid)+i,this.height*1.33/3, 2.0 +((i % 15) == 0 ?2.0:0.0), 11.0);
        if ( i < signal2)
	    this.signalStrength2[i].attr("fill","#0000FF");
        else
	    this.signalStrength2[i].attr("fill","#555555");
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
  updateIF : function(channel) {
    var grid = 4;
    var axisx1 = (this.width*0.75/grid);
    var axisx2 = (this.width*0.25/grid);
    var axiswidth = axisx1 - axisx2;
    var pos = 0;
    if (channel == 1) {
        pos = ((this.ifshift1-128)*axiswidth/255.0);
        this.IF1.transform("t"+(pos)+",0");
    } else {
        axisx1 = (this.width*2.75/grid);
        axisx2 = (this.width*2.25/grid);
        pos = ((this.ifshift2-128)*axiswidth/255.0);
        this.IF2.transform("t"+(pos)+",0");
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
    this.IF1 = paper.path("M "+(axisx2 + pos - (filterwidth*axiswidth/255.0) - (axiswidth/8.0))+" "+axisy2+" l "+(axiswidth/8.0)+" "+(axisheight * 4 /8)+" l "+(filterwidth*axiswidth/255.0)*2+" 0 l "+(axiswidth/8.0)+" "+(-1*axisheight * 4 /8));
    this.IF1.attr({stroke:"#0000FF"});
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
    this.IF2 = paper.path("M "+(axisx2 + pos - (filterwidth*axiswidth/255.0) - (axiswidth/8.0))+" "+axisy2+" l "+(axiswidth/8.0)+" "+(axisheight * 4 /8)+" l "+(filterwidth*axiswidth/255.0)*2+" 0 l "+(axiswidth/8.0)+" "+(-1*axisheight * 4 /8));
    this.IF2.attr({stroke:"#0000FF"});
    var label2 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label2.attr("fill","#9999FF");
    var t2 = paper.text(axismid,axisy2 +18.0, "IF");
    t2.attr({ fill:"#000000",'font-size':15});
  },
  UpdateFilter: function(channel) {
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
    if (channel == 1) {
        this.filterWidth1.remove();
        this.filterWidth1 = this.paper.path("M "+(axisx2 + pos - (Filter1*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter1*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
        this.filterText1.attr("text", this.filter1/1000+"k");
        this.filterWidth1.attr({stroke:"#0000FF"});
    } else {
        axisx1 = (this.width*3.75/grid);
        axisx2 = (this.width*3.25/grid);
        axismid = axisx1 + (axisx2 - axisx1)/2.0;
        this.filterWidth2.remove();
        this.filterWidth2 = this.paper.path("M "+(axisx2 + pos - (Filter2*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter2*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
        this.filterText2.attr("text",this.filter2/1000+"k");
        this.filterWidth2.attr({stroke:"#0000FF"});
    }
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
    this.filterWidth1 = paper.path("M "+(axisx2 + pos - (Filter1*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter1*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
    this.filterWidth1.attr({stroke:"#0000FF"});
    var label1 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label1.attr("fill","#9999FF");
    this.filterText1 = paper.text(axismid,axisy2 +18.0, Filter1/1000+"k");
    this.filterText1.attr({ fill:"#000000",'font-size':15});

    // Second filter
    axisx1 = (this.width*3.75/grid);
    axisx2 = (this.width*3.25/grid);
    axismid = axisx1 + (axisx2 - axisx1)/2.0;
    pos = 128*axiswidth/255.0;

    var ax2 = paper.path("M "+axisx2+" "+axisy2+" l "+axiswidth+" 0 z");
    ax2.attr({stroke:"#AAAAAA"});
    var ay2 = paper.path("M "+axismid+" "+axisy2+" l 0 "+axisheight+" z");
    ay2.attr({ 'stroke-dasharray':"-",stroke:"#AAAAAA"});
    this.filterWidth2 = paper.path("M "+(axisx2 + pos - (Filter2*axiswidth/460000.0) - (axiswidth/16.0))+" "+axisy2+" l "+(axiswidth/16.0)+" "+(axisheight * 4 /8)+" l "+(Filter2*axiswidth/460000.0)*2+" 0 l "+(axiswidth/16.0)+" "+(-1*axisheight * 4 /8));
    this.filterWidth2.attr({stroke:"#0000FF"});
    var label2 = paper.rect(axismid - axiswidth/4.0,axisy2 +8.0,axiswidth * 2 / 4,20);
    label2.attr("fill","#9999FF");
    this.filterText2 = paper.text(axismid,axisy2 +18.0, Filter2/1000+"k");
    this.filterText2.attr({ fill:"#000000",'font-size':15});
  },
  drawStepSize: function(paper) {
    var left = this.width/4;
    var right = this.width*3/4;
    var top = this.height*2/3;
    var label1 = paper.rect(left - 25.0, top - 10.0, 50.0, 20.0);
    label1.attr("fill","#9999FF");
    var step1 = this.stepsize1/1000;
    this.StepSize1 = paper.text(left,top, ((step1>=1000)? step1/1000+"M":step1+"k"));
    this.StepSize1.attr({ fill:"#000000",'font-size':15});

    var label2 = paper.rect(right - 25.0, top - 10.0, 50.0, 20.0);
    label2.attr("fill","#9999FF");
    var step2 = this.stepsize2/1000;
    this.StepSize2 = paper.text(right,top, ((step2>=1000)? step2/1000+"M":step2+"k"));
    this.StepSize2.attr({ fill:"#000000",'font-size':15});
  },
  drawMode: function(paper) {
    var left = this.width/4;
    var right = this.width*3/4;
    var top = this.height*2.3/3;
    var label1 = paper.rect(left - 25.0, top - 10.0, 50.0, 20.0);
    label1.attr("fill","#9999FF");
    this.mode1 = paper.text(left,top, this.modulation1);
    this.mode1.attr({ fill:"#000000",'font-size':15});

    var label2 = paper.rect(right - 25.0, top - 10.0, 50.0, 20.0);
    label2.attr("fill","#9999FF");
    this.mode2 = paper.text(right,top, this.modulation2);
    this.mode2.attr({ fill:"#000000",'font-size':15});
  },
  drawOptions : function(paper) {
    var left = this.width/2;
    var top = 0;
    top = this.height*3/8;
    this.AGC = paper.rect(left - 15.0, top - 10.0, 50.0, 20.0);
    this.AGC.attr("fill","#000000");
    var t1 = paper.text(left+5.0,top, "AGC");
    t1.attr({ fill:"#000000",'font-size':15});
    top = this.height*4/8;
    this.NB = paper.rect(left - 15.0, top - 10.0, 50.0, 20.0);
    this.NB.attr("fill","#000000");
    var t1 = paper.text(left+5.0,top, "NB");
    t1.attr({ fill:"#000000",'font-size':15});
    top = this.height*5/8;
    this.VSC = paper.rect(left - 15.0, top - 10.0, 50.0, 20.0);
    this.VSC.attr("fill","#000000");
    var t1 = paper.text(left+5.0,top, "VSC");
    t1.attr({ fill:"#000000",'font-size':15});
  },
  draw: function(paper) {
    // clear all
    paper.clear();
    var background = paper.rect( 10, 10, this.width, this.height);
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
    // draw options
    this.drawOptions(paper);
  },
  setFilter: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	   this.filter1 = model.get("filter1");
    else if (radio == 2)
	   this.filter2 = model.get("filter2");
    this.UpdateFilter(radio);
  },
  setSignal: function(model) {
    //var radio = model.get("radio");
    //if (radio == 1)
    this.signal1 = model.get("signal1");
    //else if (radio == 2)
    this.signal2 = model.get("signal2");
    for (var i=0; i < 255; i+=5) {
        // signal1
        if ( i < this.signal1)
            this.signalStrength1[i].attr("fill","#0000FF");
        else
            this.signalStrength1[i].attr("fill","#555555");
        // signal2
        if ( i < this.signal2)
            this.signalStrength2[i].attr("fill","#0000FF");
        else
            this.signalStrength2[i].attr("fill","#555555");
    }    
  },
  setIFShift: function(model) {
    var radio = model.get("radio");
    if (radio == 1)
	   this.ifshift1 = model.get("ifshift1");
    else if (radio == 2)
	   this.ifshift2 = model.get("ifshift2");
    this.updateIF(radio);
  },
  setModulation: function(model) {
    var radio = model.get("radio");
    if (radio == 1) {
	   this.modulation1 = model.get("modulation1");
       this.mode1.attr('text',this.modulation1);
    }
    else if (radio == 2) {
	   this.modulation2 = model.get("modulation2");
       this.mode2.attr('text',this.modulation2);
    }
  },
  setRadio: function(model) {
    var activate = "#0000FF";
    var deactivate = "#555555";
    // Change radio
    this.radio = model.get("radio");
    // Restore FreqTable model
    if (this.radio==1) {
      this.freqTable.setPositionFromValue(this.stepsize1);
    }
    else {
      this.freqTable.setPositionFromValue(this.stepsize2);
    }
    this.freq1.attr({fill:this.radio==1?activate:deactivate});
    this.freq2.attr({fill:this.radio==2?activate:deactivate});
  },
  setFrequency: function(model) {
    var radio = model.get("radio");
    if (radio ==1) {
      this.frequency1 = model.get("frequency1");
      this.freq1.attr('text',this.frequency1);
    }
    if (radio ==2) {
      this.frequency2 = model.get("frequency2");
      this.freq2.attr('text',this.frequency2);
    }
  },
  changeFrequency: function(e) {
    if (this.skipChange == true) {
        return;
    }
    // cross-browser wheel delta
    var e = window.event || e; // old IE support
    var delta = 0;
    if (e.originalEvent) {
        delta = Math.max(-1, Math.min(1, (e.originalEvent.wheelDelta || -e.originalEvent.detail)));
    } else {
        delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
    }
    var frequency = new Number(this.model.get("frequency"+this.model.get("radio")));
    var freq = frequency + delta * this.freqTable.get('value');
    this.model.set("frequency"+this.model.get("radio"),freq);
  },
  decreaseStep : function (e) {
    if (this.skipChange == true) {
        return;
    }
	this.freqTable.decreaseStep();
	return false;
  },
  increaseStep : function (e) {
    if (this.skipChange == true) {
        return;
    }
	this.freqTable.increaseStep();
	return false;
  },
  setStepSize: function(model) {
    var radio = model.get("control").get("radio");
    if (radio == 1) {
	   this.stepsize1 = model.get('value');
       var step1 = this.stepsize1/1000;
       this.StepSize1.attr('text',((step1>=1000)? step1/1000+"M":step1+"k"))
    }
    else if (radio == 2) {
	   this.stepsize2 = model.get('value');
       var step2 = this.stepsize2/1000;
       this.StepSize2.attr('text',((step2>=1000)? step2/1000+"M":step2+"k"))
    }
  },
  setagc : function(model) {
    this.agc = model.get("agc");
    this.AGC.attr("fill",(this.agc == true)?"#9999FF":"#000000");
  },
  setnb : function(model) {
    this.nb = model.get("nb");
    this.NB.attr("fill",(this.nb == true)?"#9999FF":"#000000");
  },
  setvsc : function(model) {
    this.vsc = model.get("vsc");
    this.VSC.attr("fill",(this.vsc == true)?"#9999FF":"#000000");
  },
  onChangeAllowed : function(model) {
    this.skipChange = !model.get("changeAllowed");
  }
});