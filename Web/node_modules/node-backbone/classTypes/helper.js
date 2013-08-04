var	Backbone = require('backbone'),
	_ = require('underscore');

exports.init = function(){

	var Helper = Backbone.Helper = function(options) {
		this.cid = _.uniqueId('Helper');
		this._configure(options || {});
		this.initialize.apply(this, arguments);
	};

	var classOptions = ['attributes'];

	_.extend(Helper.prototype, null, {

		initialize: function(){},

		_configure: function(options) {
			if (this.options) options = _.extend({}, this.options, options);
			for (var i = 0, l = classOptions.length; i < l; i++) {
				var attr = classOptions[i];
				if (options[attr]) this[attr] = options[attr];
			}
			this.options = options;
		}

	});

	var extend = function (protoProps, classProps) {
		var child = inherits(this, protoProps, classProps);
		child.extend = this.extend;
		return child;
	};

	Helper.extend = extend;

	return Helper;

}
