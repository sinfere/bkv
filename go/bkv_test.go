package bkv

import (
	"fmt"
	"strings"
	"testing"
	"time"
	"bytes"
	"encoding/hex"
)

func TestKVEncodeDecode(t *testing.T) {
	defer func() {
		// waiting for log
		time.Sleep(time.Millisecond * 50)
	}()

	testKVEncodeDecode(t, NewKVFromIntegerKey(1, []byte("dd")))
	testKVEncodeDecode(t, NewKVFromStringKey("sinfere", []byte("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd")))

	a := make([]byte, 2)
	fmt.Println(fmt.Sprintf("%v", a[2:]))
}

func testKVEncodeDecode(t *testing.T, kv *KV) {
	kvBuf, err := kv.Pack()
	if err != nil {
		t.Fatal(err)
	}

	pkv, pbf, e := UnpackKV(append(kvBuf, byte(0xFF)))
	if e != nil {
		t.Fatal(e)
	}
	if kv.isStringKey != pkv.isStringKey {
		t.Fatalf("isStringKey not equal: %v - %v", kv.isStringKey, pkv.isStringKey)
	}
	if bytes.Compare(kv.key, pkv.key) != 0  {
		t.Fatalf("key bytes not equal: %s - %s", hex.EncodeToString(kv.key), hex.EncodeToString(pkv.key))
	}
	if kv.Key() != pkv.Key() {
		t.Fatalf("key not equal: %s - %s", hex.EncodeToString(kv.key), hex.EncodeToString(pkv.key))
	}
	if bytes.Compare(kv.value, pkv.value) != 0 {
		t.Fatalf("value not equal: %s - %s", hex.EncodeToString(kv.value), hex.EncodeToString(pkv.value))
	}
	if bytes.Compare(pbf, []byte{ 0xFF }) != 0 {
		t.Fatalf("buf not equal: ff - %s", hex.EncodeToString(pbf))
	}
	fmt.Println(fmt.Sprintf("%v", hex.EncodeToString(kvBuf)))
	fmt.Println(fmt.Sprintf("%v", pkv.Key()))
}

func TestBKVEncodeDecode(t *testing.T) {
	defer func() {
		// waiting for log
		time.Sleep(time.Millisecond * 50)
	}()

	testBKVEncodeDecode(t, NewKVFromIntegerKey(1, []byte("dd")))
	testBKVEncodeDecode(t, NewKVFromStringKey("sinfere", []byte("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd")))
}

func testBKVEncodeDecode(t *testing.T, kv *KV) {
	bkv := NewBKV()
	bkv.AddKV(kv)
	bkv.AddKV(kv)
	bkvBuf, err := bkv.Pack()
	if err != nil {
		t.Fatal(err)
	}

	fmt.Println(fmt.Sprintf("%v", hex.EncodeToString(bkvBuf)))

	pbkv, pbf, _ := Unpack(append(bkvBuf, byte(0xFF)))
	pkv := pbkv.kvs[0]
	if kv.isStringKey != pkv.isStringKey {
		t.Fatalf("isStringKey not equal: %v - %v", kv.isStringKey, pkv.isStringKey)
	}
	if bytes.Compare(kv.key, pkv.key) != 0  {
		t.Fatalf("key not equal: %s - %s", hex.EncodeToString(kv.key), hex.EncodeToString(pkv.key))
	}
	if bytes.Compare(kv.value, pkv.value) != 0 {
		t.Fatalf("value not equal: %s - %s", hex.EncodeToString(kv.value), hex.EncodeToString(pkv.value))
	}
	if bytes.Compare(pbf, []byte{ 0xFF }) != 0 {
		t.Fatalf("buf not equal: ff - %s", hex.EncodeToString(pbf))
	}
}

func TestBKVEncodeDecodeKeyString(t *testing.T) {
	defer func() {
		// waiting for log
		time.Sleep(time.Millisecond * 50)
	}()

	testBKVEncodeDecodeKeyString(t, "root", []byte("dd"))
}

func testBKVEncodeDecodeKeyString(t *testing.T, key string, value []byte) {
	bkv := NewBKV()
	bkv.AddByStringKey(key, value)
	bkv.AddByStringKey(key, value)
	bkvBuf, err := bkv.Pack()
	if err != nil {
		t.Fatal(err)
	}

	fmt.Println(fmt.Sprintf("%v", hex.EncodeToString(bkvBuf)))

	pbkv, _, e := Unpack(bkvBuf)
	if e != nil {
		t.Error(e)
	}
	pkv := pbkv.kvs[0]
	if key != string(pkv.key) {
		t.Fatalf("key not equal: %s - %s", key, string(pkv.key))
	}
	if bytes.Compare(value, pkv.value) != 0 {
		t.Fatalf("value not equal: %s - %s", hex.EncodeToString(value), hex.EncodeToString(pkv.value))
	}

}

// fefe004bc7c40884747970650110011a896465766963655f696464306366356566666665383663343632138e6174747269627574655f6e616d6500060000108f6174747269627574655f76616c7565
func TestBKVDecode(t *testing.T) {
	defer func() {
		time.Sleep(time.Millisecond * 500)
	}()

	hexString := "06847479706501058473736964098870617373776f7264"
	hexString = "03010102040102006D1101033836373732363033333633393835390601045C915DE00A01050000000000000006030106010401070257040108034C040109FEC004010AFCE004010BFDAE04010C03E403010D0004010E000F04010F31380301102C030111190401120DE7040130FC29040131FF87030132010301330004013400F204013609CC03013700030138000301390003013A6404013B003903013C537D"
	//hexString = "00500020955601087d5001087f500108815001088350010885500108000000000000000000000000000000008750010889500108000000008b5001088d500108fb560108ff56010803570108075701080b5701080f57010813570108bf500108c1500108175701081b5701083fc500081f570108f3cf000823570108275701082b5701082f57010839c5000833570108375701083b5701083f570108c350010843570108"
	buf, _ := hex.DecodeString(strings.Replace(hexString, " ", "", -1))

	pbkv, _, e := Unpack(buf)
	if e != nil {
		t.Error(e)
	}

	pbkv.Dump()
}