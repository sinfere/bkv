package bkv

import (
	"encoding/binary"
	"encoding/hex"
	"errors"
	"fmt"
)

type ValueType int
const (
	ValueTypeUint8 ValueType = iota
	ValueTypeInt8
	ValueTypeUint16
	ValueTypeInt16
	ValueTypeUint32
	ValueTypeInt32
	ValueTypeUint64
	ValueTypeInt64
	ValueTypeFloat64
	ValueTypeString
	ValueTypeNumber
	ValueTypeHex
)

func ParseKVToMap(kv *KV, data map[string]interface{}, valueType ValueType, keyName string) error {
	key, value, err := ParseKV(kv, valueType)
	if err != nil {
		return err
	}
	if keyName != "" {
		key = keyName
	}
	data[key] = value
	return nil
}

func ParseKV(kv *KV, valueType ValueType) (string, interface{}, error) {
	key := ""

	if kv.IsStringKey() {
		key = kv.StringKey()
	} else {
		key = fmt.Sprintf("#%x", kv.NumberKey())
	}

	var value interface{} 
	valueBuf := kv.Value()

	switch valueType {
	case ValueTypeUint8:
		{
			if len(valueBuf) < 1 {
				return key, nil, errors.New("value length not enough")
			}
			value = valueBuf[0]
		}

	case ValueTypeInt8:
		{
			if len(valueBuf) < 1 {
				return key, nil, errors.New("value length not enough")
			}
			value = int8(valueBuf[0])
		}

	case ValueTypeUint16:
		{
			if len(valueBuf) < 2 {
				return key, nil, errors.New("value length not enough")
			}
			value = binary.BigEndian.Uint16(valueBuf)
		}

	case ValueTypeInt16:
		{
			if len(valueBuf) < 2 {
				return key, nil, errors.New("value length not enough")
			}
			value = int16(binary.BigEndian.Uint16(valueBuf))
		}

	case ValueTypeUint32:
		{
			if len(valueBuf) < 4 {
				return key, nil, errors.New("value length not enough")
			}
			value = binary.BigEndian.Uint32(valueBuf)
		}

	case ValueTypeInt32:
		{
			if len(valueBuf) < 4 {
				return key, nil, errors.New("value length not enough")
			}
			value = int32(binary.BigEndian.Uint32(valueBuf))
		}

	case ValueTypeUint64:
		{
			if len(valueBuf) < 8 {
				return key, nil, errors.New("value length not enough")
			}
			value = binary.BigEndian.Uint64(valueBuf)
		}

	case ValueTypeInt64:
		{
			if len(valueBuf) < 8 {
				return key, nil, errors.New("value length not enough")
			}
			value = int64(binary.BigEndian.Uint64(valueBuf))
		}

	case ValueTypeString:
		value = string(valueBuf)

	case ValueTypeNumber:
		value = DecodeNumber(valueBuf)

	case ValueTypeHex:
		value = hex.EncodeToString(valueBuf)

	}

	return key, value, nil
}