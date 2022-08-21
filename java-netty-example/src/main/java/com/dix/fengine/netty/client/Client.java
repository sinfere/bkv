package com.dix.fengine.netty.client;

public abstract class Client {
    String clientId;
    String sessionId;

    Session session;

    public Client(String sessionId) {
        this.sessionId = sessionId;
        this.session = new Session(this.sessionId);
    }

    public Session getSession() {
        return session;
    }

    public void setClientId(String clientId) {
        this.clientId = clientId;
        this.session.setClientId(clientId);
    }

    public abstract void write(byte[] buf);
}
