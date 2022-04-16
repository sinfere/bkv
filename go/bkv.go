package bkv

import (
	"encoding/hex"
	"errors"
	"fmt"
)

var (
	ErrKVNotExists = errors.New("kv not exists")
)

type KV struct {
	isStringKey bool
	key         []byte
	value       []byte
}

func NewKVFromIntegerKey(key uint64, value []byte) *KV {
	return &KV{
		isStringKey: false,
		key:         EncodeNumber(key),
		value:       value,
	}
}

func NewKVFromStringKey(key string, value []byte) *KV {
	keyBytes := []byte(key)
	if len(keyBytes) > 128 {
		keyBytes = keyBytes[0:128]
	}
	return &KV{
		isStringKey: true,
		key:         keyBytes,
		value:       value,
	}
}

// total length bytes + Key length byte(1 bit number / string flag + 7 bit length) + Key bytes + Value bytes

func (v *KV) Pack() ([]byte, error) {
	bs := make([]byte, 0)

	keyLen := len(v.key)
	if keyLen > 128 {
		return nil, errors.New(fmt.Sprintf("key length is bigger than 128: %v", keyLen))
	}

	totalLen := 1 + keyLen + len(v.value)

	keyLenByte := byte(keyLen & 0x7F)
	if v.isStringKey {
		keyLenByte |= 0x80
	}

	bs = append(bs, EncodeLength(uint64(totalLen))...)
	bs = append(bs, keyLenByte)
	bs = append(bs, v.key...)
	bs = append(bs, v.value...)

	return bs, nil
}

func (v *KV) Clone() *KV {
	key := make([]byte, 0)
	key = append(key, v.key...)

	value := make([]byte, 0)
	value = append(value, v.value...)

	return &KV{
		isStringKey: v.isStringKey,
		key:         key,
		value:       value,
	}
}

func (v *KV) IsStringKey() bool {
	return v.isStringKey
}

func (v *KV) StringKey() string {
	return string(v.key)
}

func (v *KV) NumberKey() uint64 {
	return DecodeNumber(v.key)
}

func (v *KV) Key() interface{} {
	if v.isStringKey {
		return v.StringKey()
	}

	return v.NumberKey()
}

func (v *KV) StringValue() string {
	return string(v.value)
}

func (v *KV) NumberValue() uint64 {
	return DecodeNumber(v.value)
}

func (v *KV) Value() []byte {
	return v.value
}

var (
	ErrorEmptyBuf = errors.New("empty buf")
)

func UnpackKV(buf []byte) (*KV, []byte, error) {
	if len(buf) == 0 {
		return nil, nil, ErrorEmptyBuf
	}

	err, totalLen, _, pb := DecodeLength(buf)
	if err != nil {
		return nil, buf, errors.New(fmt.Sprintf("decode total length fail: %v", err))
	}

	if uint64(len(pb)) < totalLen || len(pb) == 0 {
		return nil, buf, errors.New(fmt.Sprintf("totalLen = %v, len(pb) = %v", totalLen, len(pb)))
	}

	keyLenByte := pb[0]
	keyLen := uint64(keyLenByte & 0x7F)
	isStringKey := false
	if keyLenByte & 0x80 != 0 {
		isStringKey = true
	}

	if keyLen + 1 > totalLen {
		return nil, buf, errors.New(fmt.Sprintf("keyLen bigger than totalLen, keyLen=%v, totalLen=%v", keyLen, totalLen))
	}

	keyBytes := pb[1:(keyLen + 1)]
	valueBytes := pb[(keyLen + 1):totalLen]

	return &KV{
		isStringKey: isStringKey,
		key:         keyBytes,
		value:       valueBytes,
	}, pb[totalLen:], nil
}



type BKV struct {
	kvs []*KV
}



func NewBKV() *BKV {
	return &BKV{
		kvs: make([]*KV, 0),
	}
}

func Unpack(buf []byte) (*BKV, []byte, error) {
	bkv := &BKV{}

	for {
		// logger.Debug("unpacking buf: %v", hex.EncodeToString(buf))
		kv, pb, err := UnpackKV(buf)
		if err != nil {
			if err == ErrorEmptyBuf {
				return bkv, nil, nil
			}
			return bkv, pb, errors.New(fmt.Sprintf("parsing kv fail: %v", err.Error()))
		}
		if kv != nil {
			bkv.AddKV(kv)
		}
		buf = pb
	}
}

func (v *BKV) Pack() ([]byte, error) {
	bs := make([]byte, 0)

	for _, kv := range v.kvs {
		buf, err := kv.Pack()
		if err != nil {
			return nil, err
		}
		bs = append(bs, buf...)
	}

	return bs, nil
}

func (v *BKV) Items() []*KV {
	items := make([]*KV, 0)
	for _, item := range v.kvs {
		items = append(items, item.Clone())
	}
	return items
}

func (v *BKV) ContainsKey(key interface{}) bool {
	for _, item := range v.kvs {
		switch k := key.(type) {
		case uint64:
			if item.NumberKey() == k {
				return true
			}

		case string:
			if item.StringKey() == k {
				return true
			}
		}
	}

	return false
}

func (v *BKV) Dump() {
	for _, item := range v.Items() {
		fmt.Printf("kv: %v -> %v", item.Key(), hex.EncodeToString(item.Value()))
	}
}

func (v *BKV) AddKV(kv *KV) {
	v.kvs = append(v.kvs, kv)
}


func (v *BKV) AddByUInt64Key(key uint64, value []byte) {
	kv := NewKVFromIntegerKey(key, value)
	v.AddKV(kv)
}

func (v *BKV) AddByStringKey(key string, value []byte) {
	kv := NewKVFromStringKey(key, value)
	v.AddKV(kv)
}

func (v *BKV) GetKVByStringKey(key string) *KV {
	for _, item := range v.kvs {
		if item.StringKey() == key {
			return item
		}
	}
	return nil
}

func (v *BKV) GetKVByNumberKey(key uint64) *KV {
	for _, item := range v.kvs {
		if item.NumberKey() == key {
			return item
		}
	}
	return nil
}

// by string key

func (v *BKV) GetRequiredNumberValueByStringKey(key string) (uint64, error) {
	kv := v.GetKVByStringKey(key)
	if kv == nil {
		return 0, ErrKVNotExists
	}

	return kv.NumberValue(), nil
}

func (v *BKV) GetNumberValueByStringKey(key string, defaultValue uint64) uint64 {
	kv := v.GetKVByStringKey(key)
	if kv == nil {
		return defaultValue
	}

	return kv.NumberValue()
}

func (v *BKV) GetRequiredStringValueByStringKey(key string) (string, error) {
	kv := v.GetKVByStringKey(key)
	if kv == nil {
		return "", ErrKVNotExists
	}

	return kv.StringValue(), nil
}

func (v *BKV) GetStringValueByStringKey(key string, defaultValue string) string {
	kv := v.GetKVByStringKey(key)
	if kv == nil {
		return defaultValue
	}

	return kv.StringValue()
}

// by number key

func (v *BKV) GetRequiredNumberValueByNumberKey(key uint64) (uint64, error) {
	kv := v.GetKVByNumberKey(key)
	if kv == nil {
		return 0, ErrKVNotExists
	}

	return kv.NumberValue(), nil
}

func (v *BKV) GetNumberValueByNumberKey(key uint64, defaultValue uint64) uint64 {
	kv := v.GetKVByNumberKey(key)
	if kv == nil {
		return defaultValue
	}

	return kv.NumberValue()
}

func (v *BKV) GetRequiredStringValueByNumberKey(key uint64) (string, error) {
	kv := v.GetKVByNumberKey(key)
	if kv == nil {
		return "", ErrKVNotExists
	}

	return kv.StringValue(), nil
}

func (v *BKV) GetStringValueByNumberKey(key uint64, defaultValue string) string {
	kv := v.GetKVByNumberKey(key)
	if kv == nil {
		return defaultValue
	}

	return kv.StringValue()
}
