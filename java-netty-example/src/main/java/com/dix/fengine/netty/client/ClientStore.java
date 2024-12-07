package com.dix.fengine.netty.client;

public interface ClientStore {

    void updateClientSessionId(String clientId, String sessionId);

    void updateSessionClientId(String sessionId, String clientId);

    void removeBySessionId(String sessionId);

    void removeByClientId(String sessionId);

    String getSessionIdByClientId(String clientId);

    String getClientIdBySessionId(String sessionId);
}
