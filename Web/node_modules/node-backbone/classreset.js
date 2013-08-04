
var	Backbone = require('backbone'),
	_ = require('underscore');

var classTypes = require('./classTypes');

var Helper = new classTypes.Helper.init();
var View = new classTypes.View.init();
var Router = new classTypes.Router.init();

var extend = function (protoProps, classProps) {
	var child = inherits(this, protoProps, classProps);
	child.extend = this.extend;
	return child;
};

View.extend = Router.extend = Helper.extend = extend;

var ctor = function(){};

var inherits = function(parent, protoProps, staticProps) {

	var child;

	if (protoProps && protoProps.hasOwnProperty('constructor')) {
		child = protoProps.constructor;
	} else {
		child = function(){ parent.apply(this, arguments); };
	}

	_.extend(child, parent);

	ctor.prototype = parent.prototype;
	child.prototype = new ctor();

	if (protoProps) _.extend(child.prototype, protoProps);
	if (staticProps) _.extend(child, staticProps);

	child.prototype.constructor = child;
	child.__super__ = parent.prototype;

	return child;
};
