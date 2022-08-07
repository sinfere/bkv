package com.dix.fengine.netty.codec;

import com.dix.codec.bkv.BKV;
import com.dix.codec.bkv.CodecUtil;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

import javax.security.sasl.SaslServer;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;

public class Frame {
    public static final byte[] HEAD = CodecUtil.hexToBytes("AABB");
    public static final int MAX_LEN = 512;

    public static byte calculateChecksum(ByteBuf buf) {
        AtomicInteger checksum = new AtomicInteger();
        buf.forEachByte(value -> {
            checksum.addAndGet(value & 0xFF);
            return true;
        });

        return (byte)checksum.get();
    }

    public static byte[] makeFrame(BKV bkv) throws IOException {
        byte[] bkvBytes = bkv.pack();

        int len = 2 + 2 + bkvBytes.length + 1;

        ByteBuf buf = Unpooled.buffer(len);
        buf.writeBytes(HEAD);
        buf.writeShort(bkvBytes.length);
        buf.writeBytes(bkvBytes);

        ByteBuf checkSumBuf = buf.slice(2, 2 + bkvBytes.length);
        byte checksum = calculateChecksum(checkSumBuf);
        buf.writeByte(checksum);

        byte[] bytes = new byte[len];
        buf.readBytes(bytes);

        buf.release();

        return bytes;
    }

    public static byte[] makeHelloFrame() throws IOException {
        BKV bkv = new BKV();
        bkv.add(1, "hello, world");
        return Frame.makeFrame(bkv);
    }
}
