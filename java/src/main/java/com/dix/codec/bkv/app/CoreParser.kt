package com.dix.codec.bkv.app

import com.dix.codec.bkv.BKV
import com.dix.codec.bkv.CodecUtil
import java.util.*

object CoreParser {
    private val LogTag = "core-parser"

    private var remainingBuf = ByteArray(0)

    fun parse(newBuf: ByteArray) {
        var buf = concat(remainingBuf, newBuf)
        while (true) {
            if (buf.isEmpty()) {
                break
            }

            val parseResult = process(buf)
            when (parseResult.result) {
                ParseResult.ResultIncomplete -> {
                    remainingBuf = parseResult.buf!!
                    return
                }

                ParseResult.ResultInvalid -> {
                    buf = buf.copyOfRange(1, buf.size)
                }

                ParseResult.ResultFatal -> {
                    buf = ByteArray(0)
                }

                ParseResult.ResultOK -> {
                    buf = parseResult.buf!!
                    log("parse out bkv:")
                    parseResult.bkv!!.dump()
                }
            }
        }

        remainingBuf = buf
    }

    fun process(buf: ByteArray): ParseResult {
        log("parsing: ${CodecUtil.bytesToHex(buf)}")

        if (buf.size < 4) {
            log("buffer too short")
            return ParseResult(ParseResult.ResultIncomplete, buf, null)
        }

        val headIndex = indexOf(buf, byteArrayOf(0xFF.toByte()))
        if (headIndex < 0) {
            log( "header not exists")
            return ParseResult(ParseResult.ResultFatal, null, null)
        }

        val pb = buf.copyOfRange(headIndex, buf.size)
        if (pb.size < 3) {
            log("frame too short")
            return ParseResult(ParseResult.ResultIncomplete, buf, null)
        }

        val length = pb[1].toInt()
        if (pb.size < length + 2) {
            log("frame length not match, size=${pb.size}, length=$length")
            return ParseResult(ParseResult.ResultIncomplete, buf, null)
        }

        val bb = pb.copyOfRange(3, length + 2)

        val checksum = calculateChecksum(bb)
        if (checksum != pb[2]) {
            log("checksum not match, calculateChecksum=$checksum, checksum=${pb[2]}")
            return ParseResult(ParseResult.ResultInvalid, buf, null)
        }

        val unpackBKVResult = BKV.unpack(bb)
        if (unpackBKVResult.bkv.items.size == 0) {
            log("unpack bkv fail, empty items")
            return ParseResult(ParseResult.ResultInvalid, buf, null)
        }

        return ParseResult(ParseResult.ResultOK, pb.copyOfRange(length + 2, pb.size), unpackBKVResult.bkv)

    }

    private fun log(content: String) {
        println(content)
    }

    private fun concat(vararg arrays: ByteArray): ByteArray {
        var length = 0
        for (array in arrays) {
            length += array.size
        }
        val result = ByteArray(length)
        var pos = 0
        for (array in arrays) {
            System.arraycopy(array, 0, result, pos, array.size)
            pos += array.size
        }
        return result
    }

    fun indexOf(array: ByteArray, target: ByteArray): Int {
        if (target.size == 0) {
            return 0
        }

        outer@ for (i in 0 until array.size - target.size + 1) {
            for (j in target.indices) {
                if (array[i + j] != target[j]) {
                    continue@outer
                }
            }
            return i
        }

        return -1
    }

    private fun calculateChecksum(buf: ByteArray): Byte {
        var sum = 0
        for (b in buf) {
            sum += b.toUByte().toInt()
        }
        return (sum and 0xFF).toByte()
    }
}

data class ParseResult(val result: Int, val buf: ByteArray?, val bkv: BKV?) {
    companion object {
        val ResultOK = 1
        val ResultInvalid = 2
        val ResultIncomplete = 3
        val ResultFatal = 4
    }
}
