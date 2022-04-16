const frame = require("./frame");
const bkv = require("./bkv");

let b = new bkv.BKV();
b.addByStringKey("type", 1);
b.addByStringKey("ssid", "");
b.addByStringKey("password", "1");
let buffer = frame.pack(b);
console.log(buffer.length);
console.log(bkv.bufferToHex(buffer));

/**
 * @param {BKV} b
 */
function parseOutFrame(b) {
    console.log('parse out');
    console.log("")
}

frame.setParseOutFrameCallback(parseOutFrame);

let b1 = "ff19bf068474797065010584737369640a8870617373776f726431";
let b2 = "ff0b97098776";
let b3 = "657273696f6e01";
let b5 = "FF152C068474797065010C89636F6E6E5F737461740081";
let b6 = "FF2CAD0684747970650111847374696454502D4C494E4B5F39333038118870617373776F7264696D616769633838";

// frame.parse(bkv.hexToBuffer(b1));
// frame.parse(bkv.hexToBuffer(b2));
// frame.parse(bkv.hexToBuffer(b3));
// frame.parse(bkv.hexToBuffer(b5));
frame.parse(bkv.hexToBuffer(b6));



