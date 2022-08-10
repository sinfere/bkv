package com.dix.fengine.netty.core;

import java.util.concurrent.atomic.AtomicInteger;

public class Metric {
    private static AtomicInteger connCount = new AtomicInteger(0);

    public static int incrConnCount() {
        return connCount.addAndGet(1);
    }

    public static int decConnCount() {
        return connCount.addAndGet(-1);
    }

    public static int getConnCount() {
        return connCount.get();
    }
}
