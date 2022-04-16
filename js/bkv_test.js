const bkv = require("./bkv");

let value = new Uint8Array(2);
value[0] = 2;
value[1] = 3;


let b = new bkv.BKV();
b.addByNumberKey(1, value);
b.addByStringKey("version", value);
b.addByStringKey("test", "hello");

let result = b.pack();
console.log(bkv.bufferToHex(result));

let upr = bkv.BKV.unpack(result);
console.log(upr);
if (upr.code === 0) {
    console.log("unpack kv size:", upr.bkv.items().length);
    upr.bkv.dump();
} else {
    console.log("unpack fail:", upr.code)
}

