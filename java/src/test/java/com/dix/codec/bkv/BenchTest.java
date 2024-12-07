package com.dix.codec.bkv;

import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.ByteBuffer;

import static org.junit.jupiter.api.Assertions.assertEquals;

class BenchTest {

    private byte[] buffer = CodecUtil.hexToBytes("0301013103010232030103030301040403010505030106DC0601073F80000003010801040109025804010A03E815010B32343132303731333131353036383933343137");
    private byte[] stringKeyBuffer = CodecUtil.hexToBytes("0987706F6F6C5F69643109876E6F64655F6964320C8A6E6F64655F696E646578030C8A706F72745F696E646578040785706F776572050987766F6C74616765DC0C8763757272656E743F800000088673746174757301108D74696D655F636F6E73756D65640258128F656E657267795F636F6E73756D656403E8228E7472616E73616374696F6E5F696432343132303731333131353036383933343137");

    @Test
    void benchEncode() throws IOException {
        long start = System.currentTimeMillis();

        for (int i = 0; i < 1000_0000; i++) {
            encodeStringKeyBKV();
        }

        long elapsed = System.currentTimeMillis() - start;
        System.out.println(elapsed);
    }

    @Test
    void encode() throws IOException {
        System.out.println(CodecUtil.bytesToHex(encodeBKV()));
        System.out.println(CodecUtil.bytesToHex(encodeStringKeyBKV()));
    }

    private byte[] encodeBKV() throws IOException {
        BKV b = new BKV();
        b.add(0x01, "1");
        b.add(0x02, "2");
        b.add(0x03, 3);
        b.add(0x04, 4);
        b.add(0x05, 5);
        b.add(0x06, 220);
        b.add(0x07, 1.0f);
        b.add(0x08, 1);
        b.add(0x09, 600);
        b.add(0x0A, 1000);
        b.add(0x0B, "2412071311506893417");
        return b.pack();
    }

    private byte[] encodeStringKeyBKV() throws IOException {
        BKV b = new BKV();
        b.add("pool_id", "1");
        b.add("node_id", "2");
        b.add("node_index", 3);
        b.add("port_index", 4);
        b.add("power", 5);
        b.add("voltage", 220);
        b.add("current", 1.0f);
        b.add("status", 1);
        b.add("time_consumed", 600);
        b.add("energy_consumed", 1000);
        b.add("transaction_id", "2412071311506893417");
        return b.pack();
    }

    @Test
    void benchDecode() throws IOException {
        long start = System.currentTimeMillis();

        for (int i = 0; i < 1000_0000; i++) {
            decodeStringKeyBKV();
        }

        long elapsed = System.currentTimeMillis() - start;
        System.out.println(elapsed);
    }

    private String decodeBKV() throws IOException {
        UnpackBKVResult result = BKV.unpack(buffer);
        return result.getBKV().getStringValue(0x0B);
    }

    private String decodeStringKeyBKV() throws IOException {
        UnpackBKVResult result = BKV.unpack(stringKeyBuffer);
        return result.getBKV().getStringValue(0x0B);
    }
}