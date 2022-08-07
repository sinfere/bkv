package com.dix.fengine.netty.core;

import com.dix.fengine.netty.codec.BKVDecoder;
import com.dix.fengine.netty.codec.BKVEncoder;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.LengthFieldBasedFrameDecoder;
import io.netty.handler.timeout.IdleStateHandler;

import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class ServerInitializer extends ChannelInitializer<SocketChannel> {
    private ThreadPoolExecutor threadPoolExecutor;

    public ServerInitializer(ThreadPoolExecutor threadPoolExecutor) {
        this.threadPoolExecutor = threadPoolExecutor;
    }

    @Override
    public void initChannel(SocketChannel channel) throws Exception {
        ChannelPipeline cp = channel.pipeline();
        cp.addLast(new IdleStateHandler(0, 0, 6_00, TimeUnit.SECONDS));
        cp.addLast(new BKVDecoder());
        cp.addLast(new BKVEncoder());
        cp.addLast(new BKVHandler(threadPoolExecutor));
    }
}
