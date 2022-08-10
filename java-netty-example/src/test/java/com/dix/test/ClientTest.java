package com.dix.test;

import com.dix.client.NettyClient;
import com.dix.codec.bkv.CodecUtil;
import com.dix.codec.bkv.KV;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ClientTest {

    @Test
    void connect() throws IOException, InterruptedException {
        EventLoopGroup eventLoopGroup = new NioEventLoopGroup();
        NettyClient client = new NettyClient("127.0.0.1", 33000, eventLoopGroup);
        new Thread(() -> {
            try {
                client.boot();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }).start();

        TimeUnit.SECONDS.sleep(10);
    }

    @Test
    void bench1K() throws IOException, InterruptedException {
        EventLoopGroup eventLoopGroup = new NioEventLoopGroup();

        for (int i = 0; i < 10000; i++) {
            NettyClient client = new NettyClient("127.0.0.1", 33000, eventLoopGroup);
            new Thread(() -> {
                try {
                    client.boot();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }).start();

            TimeUnit.MILLISECONDS.sleep(10);
        }


        TimeUnit.SECONDS.sleep(6_00);
    }

    @Test
    void bench1K2() throws IOException, InterruptedException {
        EventLoopGroup eventLoopGroup = new NioEventLoopGroup();

        for (int i = 0; i < 10000; i++) {
            NettyClient client = new NettyClient("127.0.0.1", 33000, eventLoopGroup);
            new Thread(() -> {
                try {
                    client.boot();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }).start();

            TimeUnit.MILLISECONDS.sleep(10);
        }


        TimeUnit.SECONDS.sleep(6_00);
    }

}