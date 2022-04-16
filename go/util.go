package bkv

import (
	"errors"
)

func reverse(bs []byte) {
	for i, j := 0, len(bs) - 1; i < j; i, j = i + 1, j - 1 {
		bs[i], bs[j] = bs[j], bs[i]
	}
}

// big endian
func EncodeNumber(v uint64) []byte {
	if v == 0 {
		return []byte{ 0 }
	}

	bs := make([]byte, 0)
	for v > 0 {
		bs = append(bs, byte(v))
		v = v >> 8
	}
	reverse(bs)
	return bs
}

func DecodeNumber(buf []byte) uint64 {
	n := uint64(0)
	if len(buf) > 8 {
		buf = buf[0:8]
	}
	for _, b := range buf{
		n <<= 8
		n |= uint64(b)
	}
	return n
}

func EncodeLength(length uint64) []byte {
	bs := make([]byte, 0)
	for length > 0 {
		v := byte(length & 0x7F) | 0x80
		bs = append(bs, v)
		length = length >> 7
	}
	reverse(bs)
	lastByteIndex := len(bs) - 1
	lastByte := bs[lastByteIndex]
	lastByte &= 0x7F
	bs[lastByteIndex] = lastByte
	return bs
}

func DecodeLength(buf []byte) (error, uint64, uint64, []byte) {
	bs := make([]byte, 0)
	lengthByteSize := 0
	for _, b := range buf {
		bs = append(bs, b & 0x7F)
		lengthByteSize++
		if b & 0x80 == 0 {
			break
		}
	}
	if lengthByteSize == 0 || len(bs) > 4 {
		return errors.New("invalid length buf"), 0, 0, buf
	}

	length := uint64(0)
	for _, b := range bs {
		length <<= 7
		length |= uint64(b)
	}

	return nil, length, uint64(len(bs)), buf[lengthByteSize:]
}