module.exports = {
  render(block) {

    var attrs;
    var info_words;
    var tagname;
    var walker = block.walker();
    var event, node, entering;
    var buffer = [];
    var lastOut = "\n";
    var grandparent;
    var br = 10;

    // todo rewrite
    var out = function(s) {
      if ( typeof s === 'string' ) {
        for ( var i in s )
          buffer.push( s.charCodeAt( i ) );
      } else if ( typeof s === 'number' ) {
        buffer.push( s );
      } else {
        buffer = buffer.concat( s );
      }

      console.log(s);

      lastOut = buffer[ buffer.length - 1 ];
    };

    var cr = function() {
      if ( lastOut !== 10 )
        out(10);
    };

    var mode = 0;
    var column = 0;
    
    var addMode = function(state) {
      mode |= state;
      out([27, 33, mode]);
    }

    var removeMode = function(state) {
      mode &= ~state;
      out([27, 33, mode]);
    }

    var indent = function(c, set) {
      column = Math.max(0, (set?c : (c + column)) % 31);
      out([27, 66, column]);
    }

    var options = { time: true };

    if (options.time) { console.time("rendering"); }

    while ((event = walker.next())) {
      entering = event.entering;
      node = event.node;

      attrs = [];
      if (options.sourcepos) {
        var pos = node.sourcepos;
        if (pos) {
          attrs.push(['data-sourcepos', String(pos[0][0]) + ':' +
                String(pos[0][1]) + '-' + String(pos[1][0]) + ':' +
                String(pos[1][1])]);
        }
      }

      switch (node.type) {
      case 'Text':
        out( node.literal );
        break;
      case 'Softbreak':
        cr();
        break;
      case 'Hardbreak':
        cr();
        break;
      case 'Emph':
        // out(tag(entering ? 'em' : '/em'));
        break;
      case 'Strong':
        entering ? addMode(8) : removeMode(8);
        break;
      case 'HtmlInline':
        out('<!-- raw HTML omitted -->');
        break;
      case 'CustomInline':
        if (entering && node.onEnter)
          out(node.onEnter);
        else if (!entering && node.onExit)
          out(node.onExit);
        break;
      case 'Link':
        out(entering ? '[' : '](' + node.destination + ')');
        break;
      case 'Image':
        // bitmap
        break;
      case 'Code':
        out('\n' + node.literal + '\n');
        break;
      case 'Document':
        break;
      case 'Paragraph':
        grandparent = node.parent.parent;
        if (grandparent !== null &&
          grandparent.type === 'List') {
          if (grandparent.listTight) {
            break;
          }
        }

        cr();
        if (entering)
          out('\t');

        break;
      case 'BlockQuote':
        indent((entering)?2:-2);
        cr();
        cr();
        break;
      case 'Item':
        // TODO: list types
        console.log(node.listType);
        if (entering)
          out(8226);
        else 
          cr();
        break;

      case 'List':
        // tagname = node.listType === 'Bullet' ? 'ul' : 'ol';
        indent((entering)?2:-2);
        cr();
        break;
      case 'Heading':
        out([ ASCII_GS, 33, (entering)[0, 1, 1, 3](node.level) ]);     
        cr();
        break;
      case 'CodeBlock':
        cr();
        indent( (entering) ? 2 : -2 );
        if (entering)
          out( node.literal );
        cr();
        break;
      case 'HtmlBlock':
        cr();
        out('<!-- raw HTML omitted -->');
        cr();
        break;
      case 'CustomBlock':
        cr();
        if (entering && node.onEnter)
          out(node.onEnter);
        else if (!entering && node.onExit)
          out(node.onExit);
        cr();
        break;
      case 'ThematicBreak':
        cr();
        cr();
        break;
      default:
        throw "Unknown node type " + node.type;
      }

    }
    if (options.time) { console.timeEnd("rendering"); }
    return buffer;
  }
}