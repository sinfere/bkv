package com.dix.fengine.netty.client;

import com.dix.codec.bkv.BKV;
import com.dix.fengine.netty.app.NettyServerApp;
import com.dix.fengine.netty.codec.Frame;
import com.dix.fengine.netty.common.ErrorCode;
import com.dix.fengine.netty.common.MapUtil;
import com.dix.fengine.netty.exception.BaseException;
import com.google.common.base.Strings;
import io.netty.channel.ChannelHandlerContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

import static com.dix.fengine.netty.client.ClientUtil.getSessionId;

public class ClientRegistry {
    private static final Logger logger = LoggerFactory.getLogger(ClientRegistry.class);
    private static volatile ClientRegistry instance;

    public static ClientRegistry getInstance() {
        if (instance == null) {
            synchronized (ClientRegistry.class) {
                if (instance == null) {
                    instance = new ClientRegistry();
                }
            }
        }
        return instance;
    }

    private final ConcurrentHashMap<String, Client> clientMap = new ConcurrentHashMap<>();

    private final ClientStore clientStore = new MemoryClientStore();


    public void put(ChannelHandlerContext context) {
        String sessionId = getSessionId(context);
        clientMap.put(sessionId, new NettyClient(context));
    }

    public String getClientId(ChannelHandlerContext context) {
        String sessionId = getSessionId(context);
        return clientStore.getClientIdBySessionId(sessionId);
    }

    public void remove(ChannelHandlerContext context) {
        String sessionId = getSessionId(context);
        clientMap.remove(sessionId);
    }

    public void updateSessionClient(ChannelHandlerContext context, String clientId) {
        String sessionId = getSessionId(context);
        logger.info("update session: {} <-> {}", sessionId, clientId);
        clientStore.updateSessionClientId(sessionId, clientId);
        clientStore.updateClientSessionId(clientId, sessionId);
    }

    public Client get(String clientId) {
        String sessionId = clientStore.getSessionIdByClientId(clientId);
        if (Strings.isNullOrEmpty(sessionId)) {
            logger.warn("session not exists, session_id={}", sessionId);
            throw new BaseException(ErrorCode.SESSION_NOT_FOUND, MapUtil.toMap("session_id", sessionId));
        }

        Client client = clientMap.get(sessionId);
        if (client == null) {
            logger.warn("client not exists, session_id={}", sessionId);
            throw new BaseException(ErrorCode.CLIENT_NOT_FOUND, MapUtil.toMap("session_id", sessionId));
        }

        return client;
    }

    public void write(String clientId, byte[] buf) {
        Client client = get(clientId);
        client.write(buf);
    }

    public void write(String clientId, BKV bkv) throws IOException {
        Client client = get(clientId);
        client.write(Frame.makeFrame(bkv));
    }
}
