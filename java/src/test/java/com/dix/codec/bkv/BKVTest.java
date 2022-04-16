package com.dix.codec.bkv;

import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;

class BKVTest {
    @Test
    void pack() throws IOException {
        BKV bkv = new BKV();
        bkv.add(new KV(0x02, "Hello, world".getBytes()));
        bkv.add(new KV(0x02, new byte[]{ 0x03, 0x04, 0x05 }));
        bkv.add(new KV("dd", "012".getBytes()));
        bkv.add(new KV(99, new byte[]{ 0x03, 0x04, 0x05 }));
        assertEquals("0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405", CodecUtil.bytesToHex(bkv.pack()));
    }

    @Test
    void unpack() {
        UnpackBKVResult unpackBKVResult = BKV.unpack(CodecUtil.hexToBytes("0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405"));
        assertNotNull(unpackBKVResult);
        assertEquals(0, unpackBKVResult.getRemainingBuffer().length);

        BKV bkv = unpackBKVResult.getBKV();
        bkv.dump();

        assertTrue(bkv.containsKey(2));
        assertTrue(bkv.containsKey(99));
        assertFalse(bkv.containsKey(2.0));

        assertEquals("Hello, world", bkv.getByIndex(0).getStringValue());
        assertEquals(2, bkv.getByIndex(0).getNumberKey());

        assertEquals("030405", CodecUtil.bytesToHex(bkv.getByIndex(1).getValue()));
        assertEquals(2, bkv.getByIndex(1).getNumberKey());

        assertEquals("012", bkv.getByIndex(2).getStringValue());
        assertEquals("dd", bkv.getByIndex(2).getStringKey());

        assertEquals("030405", CodecUtil.bytesToHex(bkv.getByIndex(3).getValue()));
        assertEquals(99, bkv.getByIndex(3).getNumberKey());

//        Map<String, Object> data = new HashMap();
//        bkv.getItems().forEach(kv -> {
//            if (kv.isStringKey()) {
//                return;
//            }
//
//            long key = kv.getNumberKey();
//            switch ((int) key) {
//                case 0x1: data.put("type", ByteBuffer.wrap(kv.getValue()).getShort()); break;
//                case 0x2: data.put("request_id", ByteBuffer.wrap(kv.getValue()).getLong()); break;
//            }
//        });

//        PacketHeartbeat packetHeartbeat = new PacketHeartbeat();
//        bkv.getItems().forEach(kv -> {
//            if (!kv.isStringKey()) {
//                return;
//            }
//
//            long key = kv.getNumberKey();
//            switch ((int) key) {
//                case 0x1: packetHeartbeat.type = ByteBuffer.wrap(kv.getValue()).getShort(); break;
//                case 0x2: packetHeartbeat.requestId = ByteBuffer.wrap(kv.getValue()).getLong(); break;
//            }
//        });
    }

    @Test
    void testFloat() throws IOException {
        BKV bkv = new BKV();

        ByteBuffer byteBuffer1 = ByteBuffer.allocate(8);
        byteBuffer1.order(ByteOrder.BIG_ENDIAN);
        byteBuffer1.putFloat(0, 1.1f);
        byteBuffer1.putFloat(4, 1.2f);
        bkv.add(new KV("f1", byteBuffer1.array()));

        ByteBuffer byteBuffer2 = ByteBuffer.allocate(4);
        byteBuffer2.putFloat(0, 1.3f);
        bkv.add(new KV("f2", byteBuffer2.array()));

        String hex = CodecUtil.bytesToHex(bkv.pack());
        System.out.println("bkv: " + hex);

//        assertEquals("0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405", CodecUtil.bytesToHex(bkv.pack()));
    }

    @Test
    void testFloat2() throws IOException {
        BKV bkv = new BKV();
        bkv.add("f1", 0.1f);
        bkv.add(1, 0.2f);

        String hex = CodecUtil.bytesToHex(bkv.pack());
        System.out.println("bkv: " + hex);

        assertEquals("078266313DCCCCCD0601013E4CCCCD", CodecUtil.bytesToHex(bkv.pack()));
    }

    @Test
    void testBoolean() throws IOException {
        BKV bkv = new BKV();
        bkv.add("true", true);
        bkv.add("false", false);

        String hex = CodecUtil.bytesToHex(bkv.pack());
        System.out.println("bkv: " + hex);

        BKV nb = BKV.unpack(bkv.pack()).getBKV();

        assertEquals(true, nb.getBooleanValue("true"));
        assertEquals(false, nb.getBooleanValue("false"));

        assertEquals(1L, nb.getNumberValue("true"));
        assertEquals(0L, nb.getNumberValue("false"));

    }

    @Test
    void testNumberListStringKey() throws IOException {
        BKV bkv = new BKV();

        int size = 100;

        for (int i = 0; i < size; i++) {
            bkv.add("dd", i);
        }

        String hex = CodecUtil.bytesToHex(bkv.pack());
        System.out.println("bkv: " + hex);

        BKV nb = BKV.unpack(bkv.pack()).getBKV();

        List<Long> valueList = nb.getNumberValueList("dd");

        assertEquals(size, valueList.size());
        for (int i = 0; i < size; i++) {
            assertEquals(i, valueList.get(i));
        }
    }

    @Test
    void testNumberListNumberKey() throws IOException {
        BKV bkv = new BKV();

        int size = 100;

        for (int i = 0; i < size; i++) {
            bkv.add(0x3, i);
        }

        String hex = CodecUtil.bytesToHex(bkv.pack());
        System.out.println("bkv: " + hex);

        BKV nb = BKV.unpack(bkv.pack()).getBKV();

        List<Long> valueList = nb.getNumberValueList(0x3);

        assertEquals(size, valueList.size());
        for (int i = 0; i < size; i++) {
            assertEquals(i, valueList.get(i));
        }
    }

//    @Test
//    void testParseContextBKV() {
//        byte[] buf = CodecUtil.hexToBytes("0884636F6465363034158F6D6F7574685F73636F72655F315F31302E303030158F6D6F7574685F73636F72655F315F32302E303030158F6D6F7574685F73636F72655F325F31302E303030158F6D6F7574685F73636F72655F325F32302E3030300B84726F6C6C2D302E3733330B857069746368322E3431320983796177302E383738108E62675F736966745F6B705F636E7432138F616C6C5F736966745F6B705F636E7431333819936C6566745F73686F756C6465725F6B5F646567302E3030301A9472696768745F73686F756C6465725F6B5F646567302E303030FFFFFFA9671BFD71");
//        BKV.unpack(buf);
//    }
}

class PacketHeartbeat {
    Short type;
    Long requestId;
}