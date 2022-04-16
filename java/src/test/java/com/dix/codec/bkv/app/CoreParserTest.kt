package com.dix.codec.bkv.app

import com.dix.codec.bkv.CodecUtil
import org.junit.jupiter.api.Test

import org.junit.jupiter.api.Assertions.*

internal class CoreParserTest {

    @Test
    fun parse() {
        var hex = "FF0D15030101010301020103010301FF113603010101030102130301030403010701"
        CoreParser.parse(CodecUtil.hexToBytes(hex.replace(" ", "")))
        CoreParser.parse(CodecUtil.hexToBytes(hex.replace(" ", "")))
        CoreParser.parse(CodecUtil.hexToBytes(hex.replace(" ", "")))
    }
}