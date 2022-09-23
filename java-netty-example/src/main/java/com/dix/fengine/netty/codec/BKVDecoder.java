package com.dix.fengine.netty.codec;

import com.dix.codec.bkv.BKV;
import com.dix.codec.bkv.CodecUtil;
import com.dix.codec.bkv.UnpackBKVResult;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.ByteBufUtil;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;

/**
 * Frame layout: 0xAABB(2 byte) + LEN(2 byte, length of payload) + Payload(n byte) + Checksum(1 byte)
 */

public class BKVDecoder extends ByteToMessageDecoder {
    private static final Logger logger = LoggerFactory.getLogger(BKVDecoder.class);

    public BKVDecoder() {
    }

    @Override
    public final void decode(ChannelHandlerContext ctx, ByteBuf in, List<Object> out) throws Exception {
        String logPrefix = "decode";
        if (logger.isDebugEnabled()) {
            byte[] bytes = new byte[in.readableBytes()];
            int readerIndex = in.readerIndex();
            in.getBytes(readerIndex, bytes);
            logPrefix = "decode[" + CodecUtil.bytesToHex(bytes) + "]";
        }


        if (in.readableBytes() < 5) {
            if (logger.isDebugEnabled()) {
                logger.debug("{}: quit, length < 5", logPrefix);
            }

            return;
        }

        ByteBuf HEAD = in.alloc().buffer(2).writeBytes(Frame.HEAD);
        int headIndex = ByteBufUtil.indexOf(HEAD, in);
        HEAD.release();
        if (headIndex < 0) {
            in.discardReadBytes();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: quit, head not found", logPrefix);
            }

            return;
        }

        // reset to head
        in.readerIndex(headIndex);
        in.discardReadBytes();
        in.markReaderIndex();


        // start whole frame detect...

        if (in.readableBytes() < 5) {
            if (logger.isDebugEnabled()) {
                logger.debug("{}: quit after head positioned, length < 5", logPrefix);
            }

            return;
        }

        // read head 2 bytes
        in.readBytes(2).release();

        // read len 2 bytes
        int length = in.readShort();

        if (length <= 0 || length > Frame.MAX_LEN) {
            // invalid length, move to next decode
            in.discardReadBytes();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: invalid length(= {}, < 0 || > MAX), move to next decode", logPrefix, length);
            }

            return;
        }

        if (in.readableBytes() < length + 1) {
            // wait for more
            in.resetReaderIndex();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: wait for more after head positioned", logPrefix);
            }

            return;
        }

        ByteBuf payload = in.readBytes(length);

        ByteBuf checkSumBuf = in.alloc().buffer(2 + length);
        checkSumBuf.writeShort(length);
        ByteBuf payloadCopy = payload.copy();
        checkSumBuf.writeBytes(payloadCopy);
        payloadCopy.release();

        byte checksum = in.readByte();
        byte calculatedChecksum = Frame.calculateChecksum(checkSumBuf);
        checkSumBuf.release();
        if (calculatedChecksum != checksum) {
            // invalid frame, move to next decode
            in.resetReaderIndex();
            // skip one byte
            in.readByte();
            in.discardReadBytes();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: quit, checksum not equal, checksum={}, calculatedChecksum={}, move to next decode", logPrefix, checksum, calculatedChecksum);
            }

            payload.release();

            return;
        }

        byte[] bkvBytes = new byte[length];
        payload.readBytes(bkvBytes);
        payload.release();
        UnpackBKVResult unpackBKVResult = BKV.unpack(bkvBytes);
        BKV bkv = unpackBKVResult.getBKV();
        if (bkv != null) {
            out.add(bkv);
            in.discardReadBytes();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: bkv parse success, {}", logPrefix, CodecUtil.bytesToHex(bkvBytes));
            }

        } else {
            // invalid frame, move to next decode
            in.resetReaderIndex();
            // skip one byte
            in.readByte();
            in.discardReadBytes();

            if (logger.isDebugEnabled()) {
                logger.debug("{}: bkv parse fail, {}", logPrefix, CodecUtil.bytesToHex(bkvBytes));
            }
        }
    }
}
