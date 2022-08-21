package com.dix.fengine.netty.core;

import com.dix.codec.bkv.BKV;
import com.dix.fengine.netty.client.ClientRegistry;
import com.google.common.base.Strings;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.handler.timeout.IdleStateEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.ThreadPoolExecutor;

public class BKVHandler extends SimpleChannelInboundHandler<BKV> {

    private static final Logger logger = LoggerFactory.getLogger(BKVHandler.class);

    private final ThreadPoolExecutor serverHandlerPool;

    public BKVHandler(final ThreadPoolExecutor threadPoolExecutor) {
        this.serverHandlerPool = threadPoolExecutor;
    }

    @Override
    public void channelRead0(final ChannelHandlerContext ctx, final BKV request) {
        // filter beat ping
        request.dump();

        String clientId = request.getStringValue(0x02);
        if (!Strings.isNullOrEmpty(clientId)) {
            ClientRegistry.getInstance().updateSessionClient(ctx, clientId);
        }
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
