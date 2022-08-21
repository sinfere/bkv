package com.dix.fengine.netty.metric;

import java.util.concurrent.atomic.AtomicInteger;

public class MetricCounter {
        private final AtomicInteger count = new AtomicInteger(0);

        public int incr() {
            return count.addAndGet(1);
        }

        public int dec() {
            return count.addAndGet(-1);
        }

        public int get() {
            return count.get();
        }
    }