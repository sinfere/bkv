package com.dix.fengine.netty.bean;

import com.fasterxml.jackson.annotation.JsonInclude;

/**
 * Created by dd on 9/7/15.
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
public class ErrorResponse {
    private int code;
    private String message;
    private String desc;

    public ErrorResponse()
    {
        this(0);
    }

    public ErrorResponse(int code)
    {
        this(code, null, null);
    }

    public ErrorResponse(int code, String message) {
        this.code = code;
        this.message = message;
    }

    public ErrorResponse(int code, String message, String desc) {
        this.code = code;
        this.message = message;
        this.desc = desc;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }
}
