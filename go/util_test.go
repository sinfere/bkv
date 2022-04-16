package bkv

import (
	"bytes"
	"encoding/hex"
	"errors"
	"fmt"
	"testing"
	"time"
)

var (
	errorTestOnly = errors.New("This is a fake error.")
)

func TestEncodeDecodeLength(t *testing.T) {
	defer func() {
		// waiting for log
		time.Sleep(time.Millisecond * 50)
	}()

	testEncodeDecodeLength(t, 1)
	testEncodeDecodeLength(t, 2)
	testEncodeDecodeLength(t, 1024)
	testEncodeDecodeLength(t, 1024009)
}

func testEncodeDecodeLength(t *testing.T, i uint64) {
	b := EncodeLength(i)
	e, l, ll, pb := DecodeLength(append(b, byte(0xFF)))
	if e != nil {
		t.Fatal(e)
		return
	}
	if i != l {
		t.Fatalf("not equal: %d - %d", i, l)
	}
	if bytes.Compare(pb, []byte{ 0xFF }) != 0 {
		t.Fatalf("buf not equal: ff - %s", hex.EncodeToString(pb))
	}
	fmt.Printf("%v -> %v -> %v %v %v", i, hex.EncodeToString(b), l, ll, hex.EncodeToString(pb))
}

func TestEncodeDecodeNumber(t *testing.T) {
	defer func() {
		// waiting for log
		time.Sleep(time.Millisecond * 50)
	}()

	testEncodeDecodeNumber(t, 1)
	testEncodeDecodeNumber(t, 2)
	testEncodeDecodeNumber(t, 1024)
	testEncodeDecodeNumber(t, 1024009)
}

func testEncodeDecodeNumber(t *testing.T, i uint64) {
	b := EncodeNumber(i)
	di := DecodeNumber(b)

	if di != i {
		t.Fatalf("no equal: %v - %v", i, di)
	}
	fmt.Printf("%v -> %v", i, hex.EncodeToString(b))
}

