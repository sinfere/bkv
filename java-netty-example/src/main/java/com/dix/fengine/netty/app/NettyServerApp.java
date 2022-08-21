package com.dix.fengine.netty.app;

import com.dix.fengine.netty.core.NettyServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;

@Component
public class NettyServerApp {
    private static final Logger logger = LoggerFactory.getLogger(NettyServerApp.class);

    NettyServer nettyServer = null;

    @PostConstruct
    void boot() {
        startNettyServer();
    }

    private void startNettyServer() {
        String serverAddress = "0.0.0.0:33000";
        nettyServer = new NettyServer(serverAddress);
        try {
            logger.info("Bootstrap netty server");
            logger.info("PID={}", ProcessHandle.current().pid());
            nettyServer.start();
        } catch (Exception ex) {
            logger.error("exception: {}", ex.toString());
        }
    }

}
