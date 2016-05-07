var express = require('express');
var request = require('request');
var util = require('util');
// var constants = require('./constants.js');

var app = express();

var commonmark = require('commonmark');
var parser = commonmark.Parser();
var renderer = require('./ThermalSerialRenderer.js');

var uuid = require('node-uuid');

// ha ha ha 
var db = {
  items: {},
  printers: {
    '310032000547343138333038': {
      'id': '310032000547343138333038',
      'token': 'e218fba69bdab263db908be5b7fbb0db1976818a'
    }
  }
};

app.use(require('body-parser').json());

// respond with "hello world" when a GET request is made to the homepage
app.get('/items/:uuid', function(req, res) {
  var item = db.items[req.params.uuid];
  res.send(item || 400);
});

app.get('/items', function(req, res) {
  res.send(db.items);
});

app.post('/items', function(req, res) {
  var id = uuid.v4();
  
  db.items[id] = { id: id, message: renderer.render(parser.parse(req.body.message)) };

  // Maybe put token in header
  if ( req.body.to )
    notify( req.body.to, id, function(e, r, b) { res.send( b ); } );
  else
    res.send( db.items[id] );
});

app.post('/printers/:id/variables/:var', function(req, res) {

});

app.get('/image', function(req, res) {
  var id = uuid.v4();
  db.items[id] = { id: id, message: printBitmap(75, 75, logo) };

  notify( '310032000547343138333038', id, function(e, r, b) { res.send( b ); } );
})

function notify( printer_id, item_id, callback ) {
  var printer = db.printers[printer_id];
  request.post({
      url: util.format('https://api.particle.io/v1/devices/%s/notify?access_token=%s', printer.id, printer.token),
      json: true,
      body: {
        arg: item_id
      }
    },
    callback
  );
}

function image_pad() {

}

var maxChunkHeight = 255;

function printBitmap(w, h, data) {
  var rowBytes = Math.min(48, Math.floor((w + 7) / 8));
  var y_offset;
  var chunkHeight;
  var chunkHeightLimit;
  var x, y, i, c;

  var buffer = [];

  // Est. max rows to write at once, assuming 256 byte printer buffer.
  /// dtrEnabled
  // 384 pixels max width
  if ( true  ) {
    chunkHeightLimit = 255; // Buffer doesn't matter, handshake!
  } else {
    chunkHeightLimit = Math.floor(256 / rowBytes);

    if (chunkHeightLimit > maxChunkHeight) 
      chunkHeightLimit = maxChunkHeight;
    else if(chunkHeightLimit < 1)
      chunkHeightLimit = 1;
  }

  for ( y_offset = 0; y_offset < h; y_offset += chunkHeightLimit ) {
    chunkHeight = Math.min(h - y_offset, chunkHeightLimit);

    buffer.push(18, '*'.charCodeAt(0), chunkHeight, rowBytes);
    buffer = buffer.concat(data.slice(y_offset, y_offset + (chunkHeight * rowBytes)));
  }

  buffer.push('\n'.charCodeAt(0));

  return buffer;
}

app.listen(3000);