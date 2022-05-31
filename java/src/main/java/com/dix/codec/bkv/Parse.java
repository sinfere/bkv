package com.dix.codec.bkv;

import com.dix.codec.bkv.exception.InvalidArgException;
import com.dix.codec.bkv.exception.InvalidLengthException;
import com.dix.codec.bkv.exception.InvalidValueException;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.util.Map;

public class Parse {

    enum ValueType {
        Uint8, Int8, Uint16, Int16, Uint32, Int32, Uint64, Int64,
        Float32, Float64, String, StringTrimmed, Number, Hex
    }

    public static void inflate(KV item, Map<String, Object> map, ValueType valueType, String key) {
        if (item == null) {
            throw new InvalidArgException("kv is null");
        }

        if (map == null) {
            throw new InvalidArgException("target map is null");
        }

        if (valueType == null) {
            throw new InvalidArgException("valueType is null");
        }

        if (key == null) {
            throw new InvalidArgException("key is null");
        }

        byte[] valueBuf = item.getValue();
        Object value = null;

        switch (valueType) {
            case Uint8:
                value = toUint8(valueBuf);
                break;
            case Int8:
                value = toInt8(valueBuf);
                break;
            case Uint16:
                value = toUint16(valueBuf);
                break;
            case Int16:
                value = toInt16(valueBuf);
                break;
            case Uint32:
                value = toUint32(valueBuf);
                break;
            case Int32:
                value = toInt32(valueBuf);
                break;
            case Uint64:
                value = toUint64(valueBuf);
                break;
            case Int64:
                value = toInt64(valueBuf);
                break;
            case Float32:
                value = toFloat32(valueBuf);
                break;
            case Float64:
                value = toFloat64(valueBuf);
                break;
            case String:
                value = toString(valueBuf);
                break;
            case StringTrimmed:
                value = toString(valueBuf).replaceAll("\0+$", "");
                break;
            case Number:
                value = CodecUtil.decodeNumber(valueBuf);
                break;
            case Hex:
                value = CodecUtil.bytesToHex(valueBuf);
                break;
        }

        map.put(key, value);
    }

    public static int toUint8(byte[] valueBuf) {
        if (valueBuf.length < 1) {
            throw new InvalidLengthException();
        }

        return valueBuf[0] & 0xFF;
    }

    public static int toInt8(byte[] valueBuf) {
        if (valueBuf.length < 1) {
            throw new InvalidLengthException();
        }

        return valueBuf[0];
    }

    public static int toUint16(byte[] valueBuf) {
        if (valueBuf.length < 2) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getShort() & 0xFFFF;
    }

    public static int toInt16(byte[] valueBuf) {
        if (valueBuf.length < 2) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getShort();
    }

    public static long toUint32(byte[] valueBuf) {
        if (valueBuf.length < 4) {
            throw new InvalidLengthException();
        }

        byte[] buf8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        buf8[4] = valueBuf[0];
        buf8[5] = valueBuf[1];
        buf8[6] = valueBuf[2];
        buf8[7] = valueBuf[3];
        return ByteBuffer.wrap(buf8).getLong();
    }

    public static int toInt32(byte[] valueBuf) {
        if (valueBuf.length < 4) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getInt();
    }

    public static long toUint64(byte[] valueBuf) {
        if (valueBuf.length < 8) {
            throw new InvalidLengthException();
        }

        // unsupported real uint64, so the sign bit should not be 1 in big endian
        if ((valueBuf[0] & 0xFF) >> 7 != 0 ) {
            throw new InvalidValueException("unsupported uint64");
        }

        return ByteBuffer.wrap(valueBuf).getLong();
    }

    public static long toInt64(byte[] valueBuf) {
        if (valueBuf.length < 8) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getLong();
    }

    public static float toFloat32(byte[] valueBuf) {
        if (valueBuf.length < 4) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getFloat();
    }

    public static double toFloat64(byte[] valueBuf) {
        if (valueBuf.length < 8) {
            throw new InvalidLengthException();
        }

        return ByteBuffer.wrap(valueBuf).getDouble();
    }

    public static String toString(byte[] valueBuf) {
        return new String(valueBuf);
    }
}
