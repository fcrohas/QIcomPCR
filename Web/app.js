/**
 * Module dependencies.
 */
var http = require('http')
  , io = require('socket.io');

var HOST = '127.0.0.1'; // Icom PCR hosting machine
var CMD = 8888; // default port for command / scope data
var SOUND = 8889; // default port for sound binary data

// MVC framework 
exports.Backbone = Backbone = require('backbone');
// Application model using express
var apprequire = require('./models/appserver.js');
// Sound server model redirection
var soundserver = require('./models/soundserver.js');
// Websocket server redirection
var socketserver = require('./models/socketserver.js');
// Tcp client to icom pcr
var tcpclient = require('./models/tcpsocket.js');

// Instantiate webserver framework
var appmodel = new apprequire.AppServer();
var app = appmodel.start();

// Attach webserver framework to http server
var server = http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});

// Start websocket listener on server
var socket = io.listen(server);

// Connect to pcr sound layer
var pcr1 = new tcpclient.TcpSocket({host : HOST, port : SOUND});
var soundpcr = pcr1.connect();

// Connect to pcr command layer
var pcr2 = new tcpclient.TcpSocket({host : HOST, port : CMD});
var icompcr = pcr2.connect();

// Create the binary websocket <-> TCP socket redirection for sound channel
var sound = new soundserver.SoundServer();
sound.start(server,soundpcr);


// Create the websocket server <-> TCP socket redirection for command channel
var dataSocket = new socketserver.SocketServer({path:'/data', allowScopeFrame :true});
// start it to host
dataSocket.start(socket, icompcr);


// Create the websocket server <-> TCP socket redirection for command channel
var cmdSocket = new socketserver.SocketServer({path:'/command', allowScopeFrame : false});
// start it to host
cmdSocket.start(socket, icompcr);

