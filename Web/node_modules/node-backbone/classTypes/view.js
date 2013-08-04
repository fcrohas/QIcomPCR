var	Backbone = require('backbone'),
	_ = require('underscore');

exports.init = function(){

	var View = Backbone.View = function(options) {
		this.cid = _.uniqueId('View');
		this._configure(options || {});

		if (!this.route) {
			console.log('Error: When instantiating a new View, route was not provided.');
			return;
		}

		this.initialize.apply(this, arguments);
		this._listener();
	};

	var viewOptions = ['action', 'route'];

	_.extend(View.prototype, null, {

		initialize: function(){},

		// All views should overwrite render(). In this setup, render is called once the app is ready;
		// req and res are passed in for handling http input / output.
		render: function(req, res){
			console.log('Error : backbone_node : render function was not defined');
			res.send('Error: View not set.', 404);
		},

		_listener: function(){

			var view = this;

			global.on('ready:app', function(){
				global.trigger('ready:view', view);
			});
		},

		_configure: function(options) {
			if (this.options) options = _.extend({}, this.options, options);
			for (var i = 0, l = viewOptions.length; i < l; i++) {
				var attr = viewOptions[i];
				if (options[attr]) this[attr] = options[attr];
			}
			this.options = options;
		}

	});

	return View;

}
