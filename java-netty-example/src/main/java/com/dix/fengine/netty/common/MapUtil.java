package com.dix.fengine.netty.common;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MapUtil {
    public static Map<String, Object> toMap(Object... args) {
        if (args.length == 0 || args.length % 2 != 0) {
            return null;
        }

        Map<String, Object> map = new HashMap<>();

        for (int i = 0; i < args.length / 2; i++) {
            if (!(args[i * 2] instanceof String)) {
                return null;
            }
            String key = (String) args[i * 2];
            Object value = args[i * 2 + 1];
            map.put(key, value);
        }

        return map;
    }
}
