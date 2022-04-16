package com.dix.codec.bkv;

public class UnpackBKVResult {
    private final BKV bkv;
    private byte[] remainingBuffer;

    UnpackBKVResult(BKV bkv, byte[] remainingBuffer) {
        this.bkv = bkv;
        this.remainingBuffer = remainingBuffer;
        if (remainingBuffer == null) {
            this.remainingBuffer = new byte[]{};
        }
    }

    public byte[] getRemainingBuffer() {
        return remainingBuffer;
    }

    public BKV getBKV() {
        return bkv;
    }
}
