var	Backbone = require('backbone'),
	_ = require('underscore');

exports.init = function(){

	var Router = Backbone.Router = function(options) {
		this.cid = _.uniqueId('Router');
		this._configure(options || {});
		this.initialize.apply(this, arguments);
	};

	var routerOptions = ['attributes'];

	_.extend(Router.prototype, null, {

		initialize: function(){},

		_configure: function(options) {
			if (this.options) options = _.extend({}, this.options, options);
			for (var i = 0, l = routerOptions.length; i < l; i++) {
				var attr = routerOptions[i];
				if (options[attr]) this[attr] = options[attr];
			}
			this.options = options;
		}

	});

	return Router;

}
