package com.dix.fengine.netty.codec;

import com.dix.codec.bkv.BKV;
import com.dix.codec.bkv.CodecUtil;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.embedded.EmbeddedChannel;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.*;

class BKVDecoderTest {
    private static final Logger logger = LoggerFactory.getLogger(BKVDecoderTest.class);

    private byte[] makeFrame1() throws IOException {
        BKV bkv = new BKV();
        bkv.add(1, "hello, world");
        return Frame.makeFrame(bkv);
    }

    void assertFrame1(BKV bkv) {
        assertEquals("hello, world", bkv.getStringValue(1));
    }

    @Test
    void decode() throws IOException {
        EmbeddedChannel channel = new EmbeddedChannel(new BKVDecoder());

        logger.info("b1={}", CodecUtil.bytesToHex(makeFrame1()));

        ByteBuf b1 = Unpooled.wrappedBuffer(makeFrame1());

        logger.info("refCnt={}", b1.refCnt());

        channel.writeInbound(b1);

        logger.info("refCnt={}", b1.refCnt());

        BKV rb1 = channel.readInbound();
        assertEquals("hello", rb1.getStringValue(1));
    }

    // dirty head + frame
    @Test
    void decodeComplex1() throws IOException {
        BKVDecoder decoder = new BKVDecoder();
        EmbeddedChannel channel = new EmbeddedChannel(decoder);

        String hexBuf = CodecUtil.bytesToHex(makeFrame1());
        hexBuf = "00CCDDEE" + hexBuf;

        ByteBuf b1 = Unpooled.wrappedBuffer(CodecUtil.hexToBytes(hexBuf));

        channel.writeInbound(b1);

        BKV rb1 = channel.readInbound();
        assertFrame1(rb1);
    }

    // dirty + frame + dirty + frame
    @Test
    void decodeComplex2() throws IOException {
        BKVDecoder decoder = new BKVDecoder();
        EmbeddedChannel channel = new EmbeddedChannel(decoder);

        String hexBuf = CodecUtil.bytesToHex(makeFrame1());
        hexBuf = "00CCDDEE" + hexBuf + "00AABB0000" + hexBuf;

        ByteBuf b1 = Unpooled.wrappedBuffer(CodecUtil.hexToBytes(hexBuf));

        channel.writeInbound(b1);

        BKV rb1 = channel.readInbound();
        assertFrame1(rb1);

        BKV rb2 = channel.readInbound();
        assertFrame1(rb2);
    }

    // frame + frame + frame
    @Test
    void decodeComplex3() throws IOException, InterruptedException {
        BKVDecoder decoder = new BKVDecoder();
        EmbeddedChannel channel = new EmbeddedChannel(decoder);

        String hexBuf = CodecUtil.bytesToHex(makeFrame1());
        hexBuf = hexBuf + hexBuf + hexBuf;

        ByteBuf b1 = Unpooled.wrappedBuffer(CodecUtil.hexToBytes(hexBuf));

        channel.writeInbound(b1);

        BKV rb1 = channel.readInbound();
        assertFrame1(rb1);

        BKV rb2 = channel.readInbound();
        assertFrame1(rb2);

        BKV rb3 = channel.readInbound();
        assertFrame1(rb3);
    }
}