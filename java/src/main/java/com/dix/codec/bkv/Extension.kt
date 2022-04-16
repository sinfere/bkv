package com.dix.codec.bkv

import java.nio.ByteBuffer
import java.nio.ByteOrder

fun Int.toBKVNumber(): ByteArray {
    return CodecUtil.encodeNumber(this.toLong())
}

fun Float.toBytes(order: ByteOrder): ByteArray {
    val buffer = ByteBuffer.allocate(4)
    buffer.order(order)
    return buffer.array()
}