package com.dix.codec.bkv;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.io.IOException;

import static org.junit.jupiter.api.Assertions.*;

class KVTest {

    @Test
    void pack() throws IOException {
        assertEquals("0E010248656C6C6F2C20776F726C64", CodecUtil.bytesToHex(new KV(0x02, "Hello, world".getBytes()).pack()));
        assertEquals("0E010248656C6C6F2C20776F726C64", CodecUtil.bytesToHex(new KV(0x02, "Hello, world").pack()));
        assertEquals("050102030405", CodecUtil.bytesToHex(new KV(0x02, new byte[]{ 0x03, 0x04, 0x05 }).pack()));
        assertEquals("06826464303132", CodecUtil.bytesToHex(new KV("dd", "012".getBytes()).pack()));
        assertEquals("06826464303132", CodecUtil.bytesToHex(new KV("dd", "012").pack()));
        assertEquals("050163030405", CodecUtil.bytesToHex(new KV(99, new byte[]{ 0x03, 0x04, 0x05 }).pack()));
        assertEquals("03013001", CodecUtil.bytesToHex(new KV(0x30, 1).pack()));
    }

    @Test
    void unpack() {
        UnpackKVResult unpackKVResult1 = KV.unpack(CodecUtil.hexToBytes("0E010248656C6C6F2C20776F726C64"));
        assert unpackKVResult1 != null;
        assertEquals(0, unpackKVResult1.getRemainingBuffer().length);
        KV kv1 = unpackKVResult1.getKV();
        assertEquals(0x02L, kv1.getKey());
        assertEquals("Hello, world", kv1.getStringValue());

        UnpackKVResult unpackKVResult2 = KV.unpack(CodecUtil.hexToBytes("050102030405"));
        assert unpackKVResult2 != null;
        assertEquals(0, unpackKVResult2.getRemainingBuffer().length);
        KV kv2 = unpackKVResult2.getKV();
        assertEquals(0x02L, kv2.getKey());
        assertEquals("030405", CodecUtil.bytesToHex(kv2.getValue()));

        UnpackKVResult unpackKVResult3 = KV.unpack(CodecUtil.hexToBytes("06826464303132"));
        assert unpackKVResult3 != null;
        assertEquals(0, unpackKVResult3.getRemainingBuffer().length);
        KV kv3 = unpackKVResult3.getKV();
        assertEquals("dd", kv3.getStringKey());
        assertEquals("012", kv3.getStringValue());

        UnpackKVResult unpackKVResult4 = KV.unpack(CodecUtil.hexToBytes("050163030405"));
        assert unpackKVResult4 != null;
        assertEquals(0, unpackKVResult4.getRemainingBuffer().length);
        KV kv4 = unpackKVResult4.getKV();
        assertEquals(99, kv4.getNumberKey());
        assertEquals("030405", CodecUtil.bytesToHex(kv4.getValue()));
    }

    @Test
    void type() {
        new KV("dd", 1);
        System.out.println("-");
        System.out.println(Integer.class.getSimpleName());
        System.out.println(Long.class.getSimpleName());
        System.out.println(String.class.getSimpleName());
        byte[] a = new byte[0];
        System.out.println(a.getClass().getSimpleName());
        Boolean b = true;
        System.out.println(b.getClass().getSimpleName());
    }
}