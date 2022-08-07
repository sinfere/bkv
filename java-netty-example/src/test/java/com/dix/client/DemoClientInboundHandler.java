package com.dix.client;

import com.dix.fengine.netty.codec.Frame;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;

public class DemoClientInboundHandler extends ChannelInboundHandlerAdapter {

    private static final Logger logger = LoggerFactory.getLogger(DemoClientInboundHandler.class);

    public DemoClientInboundHandler() {

    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) {
        logger.info("channelActive: {}", ctx.channel().remoteAddress().toString());

        try {
            byte[] frameBytes = Frame.makeHelloFrame();
            ctx.writeAndFlush(ctx.alloc().buffer(frameBytes.length).writeBytes(frameBytes));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }


    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        logger.info("channelRead: {}, {}", ctx.channel().remoteAddress().toString(), msg);
        ctx.write(msg);
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) {
        logger.info("channelReadComplete: {}", ctx.channel().remoteAddress().toString());
       ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        // Close the connection when an exception is raised.
        cause.printStackTrace();
        ctx.close();
    }
}