from unittest import TestCase

from bkv import KV, BKV


def kv_to_hex(kv: KV) -> str:
    return kv.pack().hex().upper()


class TestBKV(TestCase):
    def test_pack(self):
        self.fail()

    def test_unpack(self):
        bkv = BKV.unpack(bytearray.fromhex("0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405"))
        self.assertEqual(4, len(bkv))

        self.assertEqual(bkv[0].number_key(), 2)
        self.assertEqual(bkv[0].string_value(), "Hello, world")

        self.assertEqual(bkv[1].number_key(), 2)
        self.assertEqual(bkv[1].raw_value, bytearray([3, 4, 5]))

        self.assertEqual(bkv[2].string_key(), "dd")
        self.assertEqual(bkv[2].string_value(), "012")

        self.assertEqual(bkv[3].number_key(), 99)
        self.assertEqual(bkv[3].raw_value, bytearray([3, 4, 5]))


class TestKV(TestCase):

    def test_pack(self):
        self.assertEqual(kv_to_hex(KV(2, "Hello, world")), "0E010248656C6C6F2C20776F726C64")
        self.assertEqual(kv_to_hex(KV(2, bytearray([3, 4, 5]))), "050102030405")
        self.assertEqual(kv_to_hex(KV("dd", "012")), "06826464303132")
        self.assertEqual(kv_to_hex(KV(99, bytearray([3, 4, 5]))), "050163030405")

    def test_unpack(self):
        self.fail()
