
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , http = require('http')
  , path = require('path')
  , net = require('net')
  , io = require('socket.io');

exports.Backbone = Backbone = require('backbone');

var soundserver = require('./models/soundserver.js');
var socketserver = require('./models/socketserver.js');

var app = express();

app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.set('view options', { layout: true });
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/', routes.index);
app.get('/users', user.list);     

var server = http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});

var HOST = '127.0.0.1';
var PORTCMD = 8888;
var SOUND = 8889;

// Sound tcp port connection
// this is a separate port as it is a specific thread
var soundpcr = new net.Socket();
soundpcr.connect(SOUND, HOST, function() {
  console.log('CONNECTED TO: ' + HOST + ':' + SOUND);

});

soundpcr.on('close', function() {
  console.log('Sound connection closed');
  soundpcr.destroy();
});  

var sound = new soundserver.SoundServer();
sound.start(server,soundpcr);

// Cmd/data tcp port connection
var icompcr = new net.Socket();
icompcr.connect(PORTCMD, HOST, function() {

  console.log('CONNECTED TO: ' + HOST + ':' + PORTCMD);
  // Write a message to the socket as soon as the client is connected, the server will receive it as message from the client 
  icompcr.write('I am Chuck Norris!');

});
// Add a 'close' event handler for the client socket
icompcr.on('close', function() {
  console.log('Data connection closed');
  icompcr.destroy();
});

var socket = io.listen(server);


// Create the websocket server for command channel
var dataSocket = new socketserver.SocketServer({path:'/data', allowScopeFrame :true});
// start it to host
dataSocket.start(socket, icompcr);


// Create the websocket server for command channel
var cmdSocket = new socketserver.SocketServer({path:'/command', allowScopeFrame : false});
// start it to host
cmdSocket.start(socket, icompcr);

