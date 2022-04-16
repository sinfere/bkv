package com.dix.codec.bkv;

public class DecodeLengthResult {
    private final long length;
    private byte[] remainingBuffer;

    DecodeLengthResult(long length, byte[] remainingBuffer) {
        this.length = length;
        this.remainingBuffer = remainingBuffer;
        if (remainingBuffer == null) {
            this.remainingBuffer = new byte[]{};
        }
    }

    public long getLength() {
        return length;
    }

    public byte[] getRemainingBuffer() {
        return remainingBuffer;
    }
}
