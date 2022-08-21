package com.dix.fengine.netty.client;

import io.netty.channel.ChannelHandlerContext;

public class Session {
    String clientId;
    String sessionId;

    Long inboundByteCount;
    Long outboundByteCount;

    Long lastActiveTime;
    Long createTime;

    public Session(String sessionId) {
        this.sessionId = sessionId;
    }

    public String getClientId() {
        return clientId;
    }

    public void setClientId(String clientId) {
        this.clientId = clientId;
    }

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }

    public Long getInboundByteCount() {
        return inboundByteCount;
    }

    public void setInboundByteCount(Long inboundByteCount) {
        this.inboundByteCount = inboundByteCount;
    }

    public Long getOutboundByteCount() {
        return outboundByteCount;
    }

    public void setOutboundByteCount(Long outboundByteCount) {
        this.outboundByteCount = outboundByteCount;
    }

    public Long getLastActiveTime() {
        return lastActiveTime;
    }

    public void setLastActiveTime(Long lastActiveTime) {
        this.lastActiveTime = lastActiveTime;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }
}
