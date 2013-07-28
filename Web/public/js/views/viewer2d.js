var ViewerView2D = Backbone.View.extend({
  initialize: function() {
    //this.listenTo(this.model, "change:data", this.getData);
    this.power = false;
    this.line = 0;
    this.drawdata = false;
    this.currenty = 0;
    this.currentx = 0;
    this.fixedx = -1;
    this.fixedy = -1;
    this.bandwidth = 3;
    this.channel = 0;
    this.fft = 512;
    this.render();    
  },
  //template: _.template($("#signalTemplate").html(), this.model),
  render: function() {
    this.template = _.template($("#viewerTemplate").html(), this.model);
    this.$el.html(this.template);
    this.context = canvas.getContext('2d');
    //canvas.style.width="100%";    
    this.imageData = this.context.createImageData(512, 256);
    this.overlay = this.context.createImageData(512, 256);    
    this.height = this.imageData.height;
    this.width = this.imageData.width;
    this.model.view = this;
    this.model.on('change:data', this.setData);
    this.model.on('change:channel', this.channelChange);
    return this;
  },
  events: {
            "click button.start": "powertoggle",
	    "click ul li a": "refresh",
	    "mousemove canvas" : "onMouseMove",
	    "click canvas" : "onClick",
            "DOMMouseScroll": "MouseWheelHandler",
	    "mousewheel":"MouseWheelHandler"
  },
  setData: function(model)
  {
	this.view.drawdata = true;
	var data = model.get("data");
	var imageData = this.view.overlay;
	var length = parseInt(data.substring(0,4),16); // this is the length of array
	var offset = length / 2;
	var size = length / 2;
	if (this.channel == 0) {
	  offset = 0;
	  size=length;
	  data = data.substr(4);
	}
	if (this.channel == 1) {
	  offset = 0;
	  data = data.substr(4);
	}
	if (this.channel == 2) {
	  data = data.substr(4+offset*2);
	}
	canvas.width=size;
	var lines = 1 ;	  
	for(var i=0; i<size; i++) {
		var value = parseInt(data.substr(i*2,2),16);
		index = (i + this.view.line * this.view.width) * 4;
		if (value <= 12 ) {
			imageData.data[index + 0] = 0;
			imageData.data[index + 1] = 0;
			imageData.data[index + 2] = value;
		}
		if ((value >= 12 ) && (value <= 64)) {
			imageData.data[index + 0] = 0;
			imageData.data[index + 1] = value;
			imageData.data[index + 2] = value+30;
		}
		if ((value >= 64) && (value <= 240)) {
			imageData.data[index + 0] = value+60;
			imageData.data[index + 1] = value+60;
			imageData.data[index + 2] = 0;
		}
		if (value >= 240) {
			imageData.data[index + 0] = value+30;
			imageData.data[index + 1] = 0;
			imageData.data[index + 2] = 0;
		}
		//store value before end
		imageData.data[index + 3] = 255;
	}
	this.view.context.putImageData(imageData, 0, 0);  
	// Translate vertically imageData
	if (this.view.line> this.view.height) {
	  this.view.imageData = this.view.context.getImageData(0,lines,this.view.width,this.view.line);
	  this.view.context.putImageData(this.view.imageData, 0, 0);    		  
	  // new height
	  this.view.line = this.view.line - lines-2;
	}
	// Capture for overlay
	this.view.overlay = this.view.context.getImageData(0,0,this.view.width,this.view.height);
	this.view.line = this.view.line + 1;	
	// draw overlay moving
	this.view.drawFrequency(this.view.currentx,this.view.currenty);	
	this.view.drawBandwidth(this.view.currentx,this.view.currenty);
	// draw overlay fixed
	if ((this.view.fixedx!=-1) && (this.view.fixedy!=-1)) {
	  this.view.drawFrequency(this.view.fixedx,this.view.fixedy);	
	  this.view.drawBandwidth(this.view.fixedx,this.view.fixedy);
	}
	this.view.drawdata = false;
  },
  powertoggle: function()
  {
    if (this.power == true) {
      this.model.setScope(false);
      this.power = false;
    } else {
      this.model.setScope(true);
      this.power = true;
    }
  },
  setupData: function (processing) {	
  },
  refresh: function(e) {
    var element = $(e.target);
    var rate = element.text().replace(/[^0-9.]+/g,'');
    this.model.set("scopeRate",rate); // here it is milliseconds
  },
  onMouseMove: function(event) {
    if (!this.drawdata) {
	this.currentx = event.offsetX;
	this.currenty = event.offsetY;
	this.context.putImageData(this.overlay, 0, 0);    		  
	this.drawFrequency(this.currentx,this.currenty);
	this.drawBandwidth(this.currentx,this.currenty);
	if ((this.fixedx!=-1) && (this.fixedy!=-1)) {
	  this.drawFrequency(this.fixedx,this.fixedy);
	  this.drawBandwidth(this.fixedx,this.fixedy);
	}
    }
  },
  MouseWheelHandler : function (e) {
	// cross-browser wheel delta
	var e = window.event || e; // old IE support
	var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
	// get bandwidth
	this.bandwidth += delta;
	// Redraw all
	this.onMouseMove(e);
	// Set Selected bandwidth
	this.model.set('selectedBandwidth',this.bandwidth * 22050.0 / this.fft);
  },
  onClick : function(event) {
    this.fixedx = event.offsetX;
    this.fixedy = event.offsetY;
    if ((!this.drawdata) && (this.fixedx!=-1) && (this.fixedy!=-1)) {
	this.context.putImageData(this.overlay, 0, 0);    		  
	this.drawFrequency(this.fixedx,this.fixedy);
	this.drawBandwidth(this.fixedx,this.fixedy);
    }
    // Set Selected frequency
    this.model.set('selectedFrequency',this.fixedx * 22050.0 / this.fft);
  },
  drawFrequency : function(x,y) {
      // Save image before drawing
      var ctx = this.context;
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(x,this.height);
      ctx.lineTo(x,0);
      ctx.strokeStyle = '#ff0000'; // red line
      ctx.stroke();
  },
  drawBandwidth : function(x,y) {
      // Save image before drawing
      var ctx = this.context;
      ctx.lineWidth = 1;
      ctx.beginPath();
      var side = this.bandwidth / 2.0;
      // draw left line
      ctx.strokeStyle = '#00ff00'; // red line
      ctx.moveTo(x-side,this.height);
      ctx.lineTo(x-side,0);
      ctx.stroke();
      ctx.moveTo(x+side,this.height);
      ctx.lineTo(x+side,0);
      ctx.stroke();
  },
  channelChange: function(model) {
      this.channel = model.get("channel");
      console.log(this.channel);
  }
});