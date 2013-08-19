var WheelView = Backbone.View.extend({
	initialize: function(options) {
	    this.width = options.size;
	    this.height = options.size;
	    this.label = options.label; 
	    // default range value
	    if ((options.range == undefined) || (options.range == null))
	    	options.range = 255;
	    this.range = options.range;
	    this.bindTo = options.bindTo;
        this.paper=null;
        this.ring=null;
        //this.ringWidth=5;
        this.circle=null;
        this.angle=0;
        this.delta=0;
        this.crossTimesCW=0;
        this.crossTimesCCW=0;
        this.touched=false;
        this.lastCoords ={ x: null, y: null };
		this.render();
	},
	render: function() {
	    this.template = _.template($("#wheelTemplate").html(), this.model);
	    this.$el.html(this.template);
	    //this.paper = Raphael($(this.$el).find('#wheel').get(0), this.width, this.height); 
	    this.paper = Raphael($(this.$el).find('#wheel').get(0), '100%', this.height); 
        this.ring = this.paper.circle(this.width/2, this.height/2, this.width/2 - 20);
        this.ring.attr({'stroke': '#aaa', 'stroke-width': 5, 'stroke-opacity': 0.3, fill:'r(.5,.5)#aaa-#aaa:70-#000'});
	    this.ring.attr('cx',$(this.$el).find('#wheel').width()/2);        
        this.circle = this.paper.circle(this.ring.attr('cx')+this.ring.attr('r')-10, this.ring.attr('cy'), 5);
        this.circle.attr({fill: 'r(.5,.5)#aaa-#aaa:70-#333','stroke': '#aaa', 'stroke-width': 1,});
	    this.paper.model = this.model;
	    this.paper.view = this;
        this.circle.drag(this.onDrag, this.onStart, this.onEnd);
        $(this.$el).find('#label').html(this.label).css('text-align','center');
	    return this;
	},
	events: {
	},
	toRad: function (angle) {
		return angle*Math.PI/180;
	},
	toDeg: function (angle) {
	    return angle*180/Math.PI;
	},
	onDrag : function(dx, dy, x, y) {
		var view = this.paper.view;
        var deltaX = this.ox + dx - view.ring.attr('cx'),
            deltaY = this.oy + dy - view.ring.attr('cy'),
            newAngle = (Math.atan2(deltaY, deltaX) * 180/Math.PI + 360) % 360;
        //console.log("deltaX="+deltaX+",deltaY="+deltaY+",newAngle="+newAngle);

        if (deltaX > 0 && view.touched) {
            if (deltaY > 0 && view.lastCoords.y <= 0) {
                //console.log('---CW Crossed---');
                view.crossTimesCW++;                              
            } else if (deltaY < 0 && view.lastCoords.y >= 0) {
                //console.log('---CCW Crossed---');
                view.crossTimesCCW++;
            }
        }
            
        view.delta = newAngle + 360*view.crossTimesCW - 360*view.crossTimesCCW - this.startAngle;                          
		//view.onchange(view.delta);
		view.angle = newAngle;
		view.onChange(view.angle);
		//console.log(view.angle);

		newAngle = view.toRad(newAngle);
		var newX = (view.ring.attr('r')-10)*Math.cos(newAngle) + view.ring.attr('cx'),
		   newY = (view.ring.attr('r')-10)*Math.sin(newAngle) + view.ring.attr('cy');
        this.attr({cx: newX, cy: newY});
         
        view.lastCoords.x = deltaX;
        view.lastCoords.y = deltaY;
        view.touched = true;             
    },
    onStart : function() {
    	var view = this.paper.view;
        this.ox = this.attr('cx');
        this.oy = this.attr('cy');
        
        view.lastCoords.x = this.attr('cx') - view.ring.attr('cx');
        view.lastCoords.y = this.attr('cy') - view.ring.attr('cy');

        //view.onstart();
        
        this.startAngle = view.angle;
    },
    onEnd : function() {
    	var view = this.paper.view;
        //view.onend();
        this.endAngle = view.angle;
        view.crossTimesCW = 0;
        view.crossTimesCCW = 0;
    },
    setValue : function(model) {

    },
    limit : function(value,max) {
    	return value;
    },
    onChange : function(value) {
    	// limit range
    	var range = value * this.range / 360.0;
    	//console.log('bindto='+this.bindTo+'range='+Math.round(range));
    	this.model.set(this.bindTo,Math.round(range));
    }

});