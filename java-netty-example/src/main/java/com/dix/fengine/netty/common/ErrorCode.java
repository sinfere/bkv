package com.dix.fengine.netty.common;

public class ErrorCode {

    public static ErrorItem SESSION_NOT_FOUND = new ErrorItem(1001, "session not found");
    public static ErrorItem CLIENT_NOT_FOUND = new ErrorItem(1001, "client not found");

    public static class ErrorItem {
        int code;
        String message;

        public ErrorItem(int code, String message) {
            this.code = code;
            this.message = message;
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
    }

}
