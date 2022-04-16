package com.dix.codec.bkv;

import com.dix.codec.bkv.exception.InvalidBufferException;
import com.dix.codec.bkv.exception.InvalidLengthException;

import java.io.ByteArrayOutputStream;
import java.util.Arrays;

public class CodecUtil {
    private final static char[] hexChars = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = CodecUtil.hexChars[v >>> 4];
            hexChars[j * 2 + 1] = CodecUtil.hexChars[v & 0x0F];
        }
        return new String(hexChars);
    }

    public static byte[] hexToBytes(String hex) {
        int len = hex.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(hex.charAt(i), 16) << 4) + Character.digit(hex.charAt(i + 1), 16));
        }
        return data;
    }

    private static void reverse(byte[] buf) {
        for (int i = 0, j = buf.length - 1; i < j; i++, j--) {
            byte t = buf[i];
            buf[i] = buf[j];
            buf[j] = t;
        }
    }

    public static byte[] encodeNumber(long n) {
        if (n == 0) {
            return new byte[]{ 0 };
        }

        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        while (n != 0) {
            byte v = (byte) (n & 0xFF);
            buffer.write(v);
            n = n >>> 8; // unsigned shift, use zero as filter
        }

        byte[] buf = buffer.toByteArray();
        reverse(buf);

        return buf;
    }

    public static long decodeNumber(byte[] buf) {
        long n = 0;
        if (buf.length > 8) {
            throw new InvalidBufferException();
        }

        for (byte b : buf) {
            n <<= 8;
            n |= (b & 0xFF);
        }

        return n;
    }

    public static byte[] encodeLength(long n) {
        if (n == 0) {
            throw new InvalidLengthException();
        }

        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        while (n != 0) {
            byte v = (byte) (n & 0x7F);
            v |= (byte) 0x80;
            buffer.write(v);
            n = n >>> 7; // unsigned shift, use zero as filter
        }

        byte[] buf = buffer.toByteArray();
        reverse(buf);

        int lastByteIndex = buf.length - 1;
        byte lastByte = buf[lastByteIndex];
        lastByte &= (byte) 0x7F;
        buf[lastByteIndex] = lastByte;

        return buf;
    }

    public static DecodeLengthResult decodeLength(byte[] buf) {
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();

        int lengthByteSize = 0;
        for (byte b : buf) {
            byte v = (byte) (b & 0x7f);
            buffer.write(v);
            lengthByteSize++;
            if ((b & 0x80) == 0) {
                break;
            }
        }

        if (lengthByteSize == 0 || lengthByteSize > 4) {
            throw new InvalidLengthException();
        }

        long length = 0;
        for (byte b : buffer.toByteArray()) {
            length <<= 7;
            length |= b;
        }

        byte[] remainingBuffer = Arrays.copyOfRange(buf, lengthByteSize, buf.length);

        return new DecodeLengthResult(length, remainingBuffer);
    }
}
