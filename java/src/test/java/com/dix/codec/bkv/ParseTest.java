package com.dix.codec.bkv;

import org.junit.jupiter.api.Test;

import java.nio.ByteBuffer;

import static org.junit.jupiter.api.Assertions.*;

class ParseTest {

    @Test
    void toUint64() {
        byte[] buf = ByteBuffer.allocate(8).putLong(1).array();
        System.out.println(CodecUtil.bytesToHex(buf));
        assertEquals(1, Parse.toUint64(buf));
    }
}