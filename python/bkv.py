
from typing import Union, Optional


def encode_length(i: int):
    assert type(i) is int, "i must be an integer"

    if i < 0:
        raise RuntimeError('i must be a positive integer' % i)

    buf = []
    while i > 0:
        v = (i & 0x7F) | 0x80
        buf.append(v)
        i = i >> 7

    buf.reverse()
    buf[-1] &= 0x7F

    return bytearray(buf)


def decode_length(buf: bytearray):
    bs = []
    length_byte_size = 0
    for i, v in enumerate(buf):
        bs.append(v & 0x7F)
        length_byte_size += 1
        if v & 0x80 != 0:
            break

    if length_byte_size == 0 or len(bs) > 4:
        raise RuntimeError('invalid length buf: %s' % buf.hex())

    length = 0
    for v in enumerate(bs):
        length <<= 7
        length |= v

    return length, len(bs), buf[length_byte_size:]


def encode_number(i: int) -> bytearray:
    assert type(i) is int, "i must be an integer"

    if i < 0:
        raise RuntimeError('i must be a positive integer' % i)

    if i == 0:
        return bytearray()

    buf = []
    while i > 0:
        buf.append(i & 0xFF)
        i >>= 8
    buf.reverse()

    return bytearray(buf)


def decode_number(buf: bytearray) -> int:
    i = 0
    if len(buf) > 8:
        buf = buf[0:8]

    for v in enumerate(buf):
        i <<= 8
        i |= v

    return i


def encode_key(v: Union[str, int]) -> (bool, bytearray):
    if isinstance(v, str):
        return True, bytearray(v.encode())
    elif isinstance(v, int):
        return False, encode_number(v)
    else:
        raise RuntimeError('invalid key: {}'.format(v))


def encode_value(v: Union[str, int, bytearray]) -> bytearray:
    if isinstance(v, str):
        return bytearray(v.encode())
    elif isinstance(v, int):
        return encode_number(v)
    elif isinstance(v, bytearray):
        return v
    else:
        raise RuntimeError('invalid key: {}'.format(v))


class KV:
    is_string_key = False
    key = bytearray()
    value = bytearray()

    def __init__(self, k: Optional[str, int] = None, v: Optional[str, int, bytearray] = None):
        if k is not None:
            is_string_key, key = encode_key(k)
            self.is_string_key = is_string_key
            self.key = key

        if v is not None:
            self.value = encode_value(v)

    def pack(self):
        buf = []

        key_len = len(self.key)
        if key_len > 128:
            raise RuntimeError('key is too long(%d)' % key_len)

        total_len = 1 + key_len + len(self.value)
        key_len_byte = key_len & 0x7F
        if self.is_string_key:
            key_len_byte |= 0x80

        buf.append(encode_length(total_len))
        buf.append(key_len_byte)
        buf.append(self.key)
        buf.append(self.value)

        return bytearray(buf)

    @classmethod
    def unpack(cls, bs: bytearray):
        if len(bs) == 0:
            return None, None

        total_len, _, pb = decode_length(bs)
        if len(pb) < total_len or len(pb) == 0:
            raise RuntimeError('total_len=%d, len(pb)=%d' % (total_len, len(pb)))

        key_len_byte = pb[0]
        key_len = key_len_byte & 0x7F
        is_string_key = False
        if key_len_byte & 0x80 != 0:
            is_string_key = True

        if key_len + 1 > total_len:
            raise RuntimeError('key_len bigger than total_len, key_len=%d, total_len=%d' % (key_len, total_len))

        key = pb[1:(key_len + 1)]
        value = pb[(key_len + 1):total_len]
        pending_bytes = pb[total_len:]

        kv = KV()
        kv.is_string_key = is_string_key
        kv.key = key
        kv.value = value
        return kv, pending_bytes


