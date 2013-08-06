var express = require('express')
  , routes = require('../routes')
  , user = require('../routes/user')
  , path = require('path');

exports.AppServer = Backbone.Model.extend({
	defaults : {
	},
	initialize : function() {
	},
	start : function() {
		this.app = express();
		this.app.configure(this.configure);
		this.app.configure('development', this.debugMode);
		this.app.get('/', routes.index);
		this.app.get('/users', user.list); 
		return this.app;
	},
	configure : function(){
		this.set('port', process.env.PORT || 3000);
		this.set('views', __dirname + '/views');
		this.set('view engine', 'jade');
		this.set('view options', { layout: true });
		this.use(express.favicon());
		this.use(express.logger('dev'));
		this.use(express.bodyParser());
		this.use(express.methodOverride());
		this.use(this.router);
		this.use(express.static(path.join(__dirname, '../public')));
	},
	debugMode : function(){
		this.use(express.errorHandler());
	}
});