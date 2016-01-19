var express = require('express');
var app = express();

var commonmark = require('commonmark');
var parser = commonmark.Parser();
var renderer = require('./ThermalSerialRenderer.js');

var uuid = require('node-uuid');

// ha ha ha 
var db = {};

// respond with "hello world" when a GET request is made to the homepage
app.get('/items/:uuid', function(req, res) {
  res.send(db[req.params.uuid]);
});

app.get('/items', function(req, res) {
  var id = uuid.v4();  

  db[id] = { id: id, message: renderer.render(parser.parse('Hello, and **welcome** to this\n# Here\'s a header\n\nabcdefg with `code 2 + 2` and such and so on and so forth and so on and so forth\n\n# Wow More Headers\n\n[here](google.com) is a link and so on and so forth and so on and so forth\n\n # I need sleep\n\n> here is a quote a quote a quote a quote a quote\n\n\nhere is a list:\n\n* a\n* b\n* c\n---')) };
  res.send( db[id] );
});

var event, node;


app.listen(3000);