const bkv = require("./bkv");

const ParseResultOk = 1;
const ParseResultInvalid = 2;
const ParseResultIncompleteFrame = 3;
const ParseResultFatal = 4;
let remainingBuf = new Uint8Array(0);
let parseOutFrameCallback = null;

/**
 * @param {BKV} b
 * @return {Uint8Array}
 */
function pack(b) {
    let buffer = b.pack();
    let finalBuffer = new Uint8Array(buffer.length + 3);
    finalBuffer[0] = 0xFF;
    finalBuffer[1] = buffer.length + 1;
    finalBuffer[2] = calculateChecksum(buffer);
    finalBuffer.set(buffer, 3);
    return finalBuffer;
}

function setParseOutFrameCallback(f) {
    parseOutFrameCallback = f;
}

/**
 * @param {Uint8Array} buffer
 */
function parse(buffer) {
    if (!remainingBuf) {
        remainingBuf = new Uint8Array(0);
    }

    let buf = bkv.concatenateBuffer(Uint8Array, remainingBuf, buffer);
    while (true) {
        if (!buf || buf.length === 0) {
            break;
        }

        let r = runParser(buf);
        let result = r.result;
        let pendingParseBuf = r.pendingParseBuf;
        let frame = r.frame;
        console.log('parse result: ', result);
        switch (result) {
            case ParseResultIncompleteFrame: {
                remainingBuf = pendingParseBuf;
                return;
            }

            case ParseResultInvalid: {
                buf = buf.slice(1);
            } break;

            case ParseResultFatal: {
                buf = new Uint8Array(0);
            } break;

            case ParseResultOk: {
                publishFrame(frame);
                buf = pendingParseBuf;
            } break;
        }
    }

    remainingBuf = buf;
}

/**
 *
 * @param {Uint8Array} buffer
 * @return {*}
 */
function runParser(buffer) {
    console.log('parser', 'parsing: ' + bkv.bufferToHex(buffer));

    if (buffer.length < 4) {
        console.log('parser', 'buffer too short');
        return { result: ParseResultIncompleteFrame, pendingParseBuf: buffer, frame: null }
    }

    let headIndex = buffer.findIndex((e) => e === 0xFF);
    if (headIndex < 0) {
        console.log('parser', 'head not exists');
        return { result: ParseResultFatal, pendingParseBuf: null, frame: null }
    }

    let frameBuffer = buffer.slice(headIndex);
    if (frameBuffer.length < 3) {
        console.log('parser', 'frame length too short');
        return { result: ParseResultIncompleteFrame, pendingParseBuf: buffer, frame: null }
    }

    let length = frameBuffer[1];
    if (frameBuffer.length < length + 2) {
        console.log('parser', 'frame length not match', frameBuffer.length, length + 4);
        return { result: ParseResultIncompleteFrame, pendingParseBuf: buffer, frame: null }
    }

    let checkSum = calculateChecksum(frameBuffer.slice(3, length + 2));
    if (checkSum !== frameBuffer[2]) {
        console.log('parser', 'checksum not match');
        return { result: ParseResultInvalid, pendingParseBuf: buffer, frame: null }
    }

    let bkr = bkv.BKV.unpack(frameBuffer.slice(3, length + 2));
    if (bkr.code === 0) {
        return { result: ParseResultOk, pendingParseBuf: bkr.pendingParseBuffer, frame: bkr.bkv }
    } else {
        console.log("bkv parse fail", bkr);
        return { result: ParseResultInvalid, pendingParseBuf: bkr.pendingParseBuffer, frame: bkr.bkv }
    }
}

/**
 * @param {Uint8Array} buffer
 */
function calculateChecksum(buffer) {
    let sum = 0;
    for (let i = 0; i < buffer.length; i++) {
        sum += buffer[i];
    }

    return sum & 0xFF;
}

/**
 * @param {BKV} frame
 */
function publishFrame(frame) {
    console.log('publish-frame');
    frame.dump();
    if (isFunction(parseOutFrameCallback)) {
        parseOutFrameCallback(frame);
    }
}

function isFunction(functionToCheck) {
    return functionToCheck && {}.toString.call(functionToCheck) === '[object Function]';
}

module.exports = {
    pack: pack,
    parse: parse,
    setParseOutFrameCallback: setParseOutFrameCallback,
};