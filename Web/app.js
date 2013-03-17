
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , http = require('http')
  , path = require('path')
  , io = require('socket.io')
  , net = require('net');

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

var socket = io.listen(server); 


var HOST = '127.0.0.1';
var PORT = 8888;


socket.on('connection', function (client){ 
  var icompcr = new net.Socket();
  icompcr.connect(PORT, HOST, function() {

    console.log('CONNECTED TO: ' + HOST + ':' + PORT);
    // Write a message to the socket as soon as the client is connected, the server will receive it as message from the client 
    icompcr.write('I am Chuck Norris!');

  });
  // Add a 'close' event handler for the client socket
  icompcr.on('close', function() {
    console.log('Connection closed');
    client.send('QIcomPCR closed');
    icompcr.destroy();
  });
  // Add a 'data' event handler for the client socket
  // data is what the server sent to this socket
  icompcr.on('data', function(data) {
    
    console.log('DATA: ' + data);
    client.send(data );
    
  });

  client.on('message', function () {
  }) ;

  client.on('disconnect', function () {
  });
});

