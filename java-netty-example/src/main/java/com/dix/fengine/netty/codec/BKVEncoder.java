package com.dix.fengine.netty.codec;

import com.dix.codec.bkv.BKV;
import com.dix.fengine.netty.serializer.Serializer;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToByteEncoder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Frame layout: 0xAABB(2 byte) + LEN(2 byte) + Payload(n byte) + Checksum(1 byte)
 */

public class BKVEncoder extends MessageToByteEncoder<BKV> {
    private static final Logger logger = LoggerFactory.getLogger(BKVEncoder.class);
    private Class<?> genericClass;
    private Serializer serializer;

    public BKVEncoder() {
    }

    @Override
    public void encode(ChannelHandlerContext ctx, BKV in, ByteBuf out) throws Exception {
        if (genericClass.isInstance(in)) {
            try {
                byte[] data = serializer.serialize(in);
                out.writeInt(data.length);
                out.writeBytes(data);
            } catch (Exception ex) {
                logger.error("Encode error: " + ex.toString());
            }
        }
    }
}
