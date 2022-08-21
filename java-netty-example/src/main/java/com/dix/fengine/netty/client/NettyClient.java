package com.dix.fengine.netty.client;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelHandlerContext;

public class NettyClient extends Client {
    ChannelHandlerContext context;

    public NettyClient(ChannelHandlerContext context) {
        super(ClientUtil.getSessionId(context));
        this.context = context;
    }

    @Override
    public void write(byte[] buf) {
        ByteBuf byteBuf = Unpooled.wrappedBuffer(buf);
        context.channel().writeAndFlush(byteBuf);
    }
}
