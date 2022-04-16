package com.dix.codec.bkv;

public class UnpackKVResult {
    private final KV kv;
    private byte[] remainingBuffer;

    UnpackKVResult(KV kv, byte[] remainingBuffer) {
        this.kv = kv;
        this.remainingBuffer = remainingBuffer;
        if (remainingBuffer == null) {
            this.remainingBuffer = new byte[]{};
        }
    }

    public byte[] getRemainingBuffer() {
        return remainingBuffer;
    }

    public KV getKV() {
        return kv;
    }
}
