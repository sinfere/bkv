package com.dix.fengine.netty;

import com.dix.fengine.netty.core.NettyServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Main {
    private static final Logger logger = LoggerFactory.getLogger(Main.class);

    public static void main(String[] args) {
        String serverAddress = "0.0.0.0:33000";
        NettyServer nettyServer = new NettyServer(serverAddress);
        try {
            logger.info("bootstrap netty server");
            logger.info("pid={}", ProcessHandle.current().pid());
            nettyServer.start();
        } catch (Exception ex) {
            logger.error("exception: {}", ex.toString());
        }
    }
}
