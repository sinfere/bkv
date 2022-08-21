package com.dix.fengine.netty.core;

import com.dix.fengine.netty.client.ClientRegistry;
import com.dix.fengine.netty.metric.Metric;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.timeout.IdleStateEvent;
import org.jetbrains.annotations.NotNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class LifecycleHandler extends ChannelInboundHandlerAdapter {

    private static final Logger logger = LoggerFactory.getLogger(LifecycleHandler.class);

    public LifecycleHandler() {

    }

    @Override
    public void channelActive(@NotNull ChannelHandlerContext ctx) throws Exception {
        super.channelActive(ctx);

        ClientRegistry.getInstance().put(ctx);

        int count = Metric.getInstance().connectionCount.incr();
        logger.info("[channelActive] con count: {}", count);
    }

    @Override
    public void channelInactive(@NotNull ChannelHandlerContext ctx) throws Exception {
        super.channelInactive(ctx);

        ClientRegistry.getInstance().remove(ctx);

        int count = Metric.getInstance().connectionCount.dec();
        logger.info("[channelInactive] con count: {}", count);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        logger.warn("Server caught exception: " + cause.getMessage());
        ctx.close();
    }

    @Override
    public void userEventTriggered(ChannelHandlerContext ctx, Object evt) throws Exception {
        if (evt instanceof IdleStateEvent) {
            ctx.channel().close();
        } else {
            super.userEventTriggered(ctx, evt);
        }
    }
}
