package com.dix.client;

import com.dix.codec.bkv.BKV;
import com.dix.fengine.netty.codec.Frame;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.*;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import org.jetbrains.annotations.NotNull;

import java.io.IOException;

public class NettyClient {
    private final String host;
    private final int port;
    private final EventLoopGroup eventLoopGroup;

    public NettyClient(String host, int port, EventLoopGroup eventLoopGroup) {
        this.host = host;
        this.port = port;
        this.eventLoopGroup = eventLoopGroup;
    }

    public void boot() throws InterruptedException {
        Bootstrap b = new Bootstrap();
        b.group(eventLoopGroup)
                .channel(NioSocketChannel.class)
                .option(ChannelOption.TCP_NODELAY, true)
                .handler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    public void initChannel(@NotNull SocketChannel ch) throws Exception {
                        ChannelPipeline p = ch.pipeline();
                        p.addLast(new LoggingHandler(LogLevel.INFO));
                        p.addLast(new DemoClientInboundHandler());
                        p.addLast(new DemoClientOutboundHandler());
                    }
                });

        // Start the client.
        ChannelFuture f = b.connect(this.host, this.port).sync();

        // Wait until the connection is closed.
        f.channel().closeFuture().sync();
    }


}
