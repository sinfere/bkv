package com.dix.fengine.netty.client;

import com.google.common.base.Strings;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class MemoryClientStore implements ClientStore {
    // session related
    private final ConcurrentHashMap<String, String> sessionClientMap = new ConcurrentHashMap<>();

    // client related
    private final ConcurrentHashMap<String, String> clientSessionMap = new ConcurrentHashMap<>();

    @Override
    public void updateClientSessionId(String clientId, String sessionId) {
        if (Strings.isNullOrEmpty(clientId)) {
            return;
        }

        if (Strings.isNullOrEmpty(sessionId)) {
            return;
        }

        clientSessionMap.put(clientId, sessionId);
    }

    @Override
    public void updateSessionClientId(String sessionId, String clientId) {
        if (Strings.isNullOrEmpty(sessionId)) {
            return;
        }

        if (Strings.isNullOrEmpty(clientId)) {
            return;
        }

        sessionClientMap.put(sessionId, clientId);
    }

    @Override
    public void removeBySessionId(String sessionId) {
        if (Strings.isNullOrEmpty(sessionId)) {
            return;
        }

        sessionClientMap.remove(sessionId);

        for (Map.Entry<String, String> entry : clientSessionMap.entrySet()) {
            if (entry.getValue().equals(sessionId)) {
                clientSessionMap.remove(entry.getKey());
            }
        }
    }

    @Override
    public void removeByClientId(String clientId) {
        if (Strings.isNullOrEmpty(clientId)) {
            return;
        }

        clientSessionMap.remove(clientId);

        for (Map.Entry<String, String> entry : sessionClientMap.entrySet()) {
            if (entry.getValue().equals(clientId)) {
                sessionClientMap.remove(entry.getKey());
            }
        }
    }

    @Override
    public String getSessionIdByClientId(String clientId) {
        if (Strings.isNullOrEmpty(clientId)) {
            return null;
        }

        return clientSessionMap.get(clientId);
    }
}
