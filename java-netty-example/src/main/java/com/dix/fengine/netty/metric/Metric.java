package com.dix.fengine.netty.metric;

public class Metric {
    private static volatile Metric instance;

    public static Metric getInstance() {
        if (instance == null) {
            synchronized (Metric.class) {
                if (instance == null) {
                    instance = new Metric();
                }
            }
        }
        return instance;
    }
    public MetricCounter connectionCount = new MetricCounter();

}
