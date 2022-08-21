package com.dix.fengine.netty.exception;

import com.dix.fengine.netty.common.ErrorCode;

import java.util.HashMap;
import java.util.Map;

public class BaseException extends RuntimeException
{
    public static final int ERROR_404                   = -404;
    public static final int ERROR_500                   = -500;
    public static final int ERROR                       = -1;
    public static final int ERROR_IN_INTERCEPTOR        = -2;

    public static final int ERROR_PARAM_NOT_SET = 1;
    public static final int ERROR_AUTH_FAIL = 2;
    public static final int ERROR_LOGIN = 3;
    public static final int ERROR_WRONG_PARAM = 4;
    public static final int ERROR_NOT_EXISTS = 5;
    public static final int ERROR_EXISTS = 6;
    public static final int ERROR_NOT_ALLOWED = 7;

    private int code;
    private String message;

    private Map<String, Object> data;

    public int getCode() {
        return code;
    }

    @Override
    public String getMessage() {
        return message;
    }

    public BaseException(int code, String message)
    {
        super("" + code + ": " + message);
        this.code = code;
        this.message = message;
    }

    public BaseException(String message) {
        this(-1, message);
    }

    public BaseException(ErrorCode.ErrorItem item) {
        this(item.getCode(), item.getMessage());
    }

    public BaseException(ErrorCode.ErrorItem item, Map<String, Object> data) {
        this(item.getCode(), item.getMessage());
        this.data = new HashMap<>();
        this.data.putAll(data);
    }
}