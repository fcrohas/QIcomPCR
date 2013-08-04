
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , http = require('http')
  , path = require('path')
  , io = require('socket.io')
  , net = require('net')
  , binaryjs = require('binaryjs')
  , _ = require('underscore')._
  , backbone = require('backbone');

var app = express();
var BinaryServer = binaryjs.BinaryServer;

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

var binarySocket = new BinaryServer({ server: server, path :'/stream' });
var socket = io.listen(server); 
// Hack for binary js to work
socket.set("destroy upgrade",false);


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

socket.of('/command').on('connection', function (client){ 
  // Add a 'data' event handler for the client socket
  // data is what the server sent to this socket
  icompcr.on('data', function(data) {
    // discard frame starting with WT
    var msg = new String(data);
    // try to find if multiple frame are here
    var frame = msg.split('@');
    for (var i=0; i < frame.length; i++) {
      if (frame[i] !='') {
	if (frame[i].substring(0,2) != 'WT') {
	  client.volatile.send(frame[i]);
	}
      }
    }
  });

  client.on('message', function (msg) {
   icompcr.write(msg);
  });

  client.on('disconnect', function () {
  });
});

socket.of('/data').on('connection', function (client){ 
  // Add a 'data' event handler for the client socket
  // data is what the server sent to this socket
  icompcr.on('data', function(data) {
    // Allow only waterfall frame 
    var msg = new String(data);    
    // try to find if multiple frame are here
    var frame = msg.split('@');
    for (var i=0; i < frame.length; i++) {
      if (frame[i] !='') {
	if (frame[i].substring(0,2) == 'WT') {
	  client.volatile.send(frame[i]);
	}
      }
    }
  });

  client.on('message', function (msg) {
    icompcr.write(msg);
  });

  client.on('disconnect', function () {
  });
});

binarySocket.on('connection', function(client) {
  console.log('Someone connected!');
  client.on('stream', function(stream) {
    console.log('client stream started!');
  });

  soundpcr.on('data', function(data) {
    client.send(data);
  });

});
