package main

import (
	"bytes"
	"testing"
)

func TestEncodeCommand(t *testing.T) {
	got := encodeCommand(cmdUpdate, 0x12345678)
	want := []byte{0xAA, 0x55, 0x04, 0x00, 0x78, 0x56, 0x34, 0x12}
	if !bytes.Equal(got, want) {
		t.Fatalf("encodeCommand = % X, want % X", got, want)
	}
}

func TestEncodeTail(t *testing.T) {
	got := encodeTail(0xDEADBEEF)
	want := []byte{0xEF, 0xBE, 0xAD, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF}
	if !bytes.Equal(got, want) {
		t.Fatalf("encodeTail = % X, want % X", got, want)
	}
}

func TestDecodeReply(t *testing.T) {
	r, err := decodeReply([]byte{0xAA, 0x55, 0x03, 0x00, 0x07, 0x00, 0x02, 0x00})
	if err != nil {
		t.Fatal(err)
	}
	if r.Cmd != cmdStart || r.Status != statusDataErr || r.Errno != 2 {
		t.Fatalf("decodeReply = %+v", r)
	}
	if r.OK() {
		t.Fatal("reply with non-zero status reported OK")
	}
	if _, err := decodeReply([]byte{1, 2, 3, 4, 5, 6, 7, 8}); err == nil {
		t.Fatal("bad magic accepted")
	}
	if _, err := decodeReply([]byte{0xAA, 0x55}); err == nil {
		t.Fatal("short frame accepted")
	}
}

func TestAppendFrameResync(t *testing.T) {
	// Garbage, a false start (AA followed by non-55), then a real frame
	// split across two reads.
	var f []byte
	f = appendFrame(f, []byte{0x00, 0xAA, 0x01, 0xAA, 0x55, 0x02})
	if len(f) != 3 {
		t.Fatalf("partial frame length = %d, want 3", len(f))
	}
	f = appendFrame(f, []byte{0x00, 0x00, 0x00, 0x00, 0x00})
	if len(f) != frameSize {
		t.Fatalf("frame length = %d, want %d", len(f), frameSize)
	}
	r, err := decodeReply(f)
	if err != nil || r.Cmd != cmdStop {
		t.Fatalf("decoded %+v, err %v", r, err)
	}
}
