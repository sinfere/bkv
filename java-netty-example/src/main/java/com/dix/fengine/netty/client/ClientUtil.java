package com.dix.fengine.netty.client;

import io.netty.channel.ChannelHandlerContext;

public class ClientUtil {
    public static String getSessionId(ChannelHandlerContext context) {
        return context.channel().remoteAddress().toString();
    }
}
