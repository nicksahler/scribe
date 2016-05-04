var express = require('express');

var app = express();

var commonmark = require('commonmark');
var parser = commonmark.Parser();
var renderer = require('./ThermalSerialRenderer.js');

var uuid = require('node-uuid');

// ha ha ha 
var db = {};

app.use(require('body-parser').json());

// respond with "hello world" when a GET request is made to the homepage
app.get('/items/:uuid', function(req, res) {
  res.send(db[req.params.uuid]);
});

app.post('/items', function(req, res) {
  var id = uuid.v4();
  
  db[id] = { id: id, message: renderer.render(parser.parse(req.body.message)) };
  res.send( db[id] );
});

var event, node;


app.listen(3000);