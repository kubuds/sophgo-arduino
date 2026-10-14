package main

import (
	"bytes"
	"encoding/binary"
	"hash/crc32"
	"math/rand"
	"net"
	"strings"
	"sync"
	"testing"
	"time"
)

// pipeTransport adapts one end of net.Pipe to the transport interface.
type pipeTransport struct{ net.Conn }

func (p pipeTransport) SetReadTimeout(d time.Duration) error {
	return p.Conn.SetReadDeadline(time.Now().Add(d))
}

// fakeBurnd emulates the board daemon: it reads the port in 64-byte slices
// exactly like burnd and answers each command. Options let tests inject
// failures.
type fakeBurnd struct {
	status map[command]uint16 // reply status per command, default OK
	silent bool               // never answer
	badCRC bool               // report a CRC mismatch for the tail

	mu       sync.Mutex
	received []byte
	cmds     []command
}

func (f *fakeBurnd) serve(conn net.Conn) {
	defer conn.Close()
	buf := make([]byte, 64)
	var updating bool
	var want uint32
	var data []byte
	for {
		n, err := conn.Read(buf)
		if err != nil {
			return
		}
		b := buf[:n]
		if f.silent {
			continue
		}

		if !updating && n == frameSize && b[0] == 0xAA && b[1] == 0x55 {
			cmd := command(binary.LittleEndian.Uint16(b[2:]))
			f.mu.Lock()
			f.cmds = append(f.cmds, cmd)
			f.mu.Unlock()
			if cmd == cmdUpdate {
				updating, want, data = true, binary.LittleEndian.Uint32(b[4:]), nil
			}
			conn.Write(fakeReply(cmd, f.status[cmd]))
			continue
		}
		if !updating {
			continue
		}
		if uint32(len(data)) < want {
			data = append(data, b...)
			continue
		}
		if n == frameSize && binary.LittleEndian.Uint32(b[4:]) == frameTailMarker {
			status := statusOK
			if f.badCRC || binary.LittleEndian.Uint32(b[0:]) != crc32.ChecksumIEEE(data) {
				status = statusDataErr
			}
			f.mu.Lock()
			f.received = data
			f.mu.Unlock()
			updating = false
			conn.Write(fakeReply(cmdUpdate, status))
		}
	}
}

func fakeReply(cmd command, status uint16) []byte {
	b := make([]byte, frameSize)
	b[0], b[1] = 0xAA, 0x55
	binary.LittleEndian.PutUint16(b[2:], uint16(cmd))
	binary.LittleEndian.PutUint16(b[4:], status)
	return b
}

func newTestClient(t *testing.T, f *fakeBurnd) *Client {
	t.Helper()
	host, board := net.Pipe()
	go f.serve(board)
	t.Cleanup(func() { host.Close() })
	c := NewClient(pipeTransport{host})
	c.ReplyTimeout = 300 * time.Millisecond
	c.UpdateTimeout = 300 * time.Millisecond
	c.TailDelay = 0
	c.Logf = t.Logf
	return c
}

func randomFirmware(n int) []byte {
	b := make([]byte, n)
	rand.New(rand.NewSource(1)).Read(b)
	return b
}

func TestUploadSuccess(t *testing.T) {
	f := &fakeBurnd{}
	c := newTestClient(t, f)
	var last int
	c.Progress = func(sent, total int) { last = sent }

	fw := randomFirmware(5000) // not a multiple of 64 nor of ChunkSize
	if err := c.Upload(fw); err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(f.received, fw) {
		t.Fatalf("board received %d bytes, want %d identical bytes", len(f.received), len(fw))
	}
	if last != len(fw) {
		t.Fatalf("progress stopped at %d of %d", last, len(fw))
	}
	want := []command{cmdNone, cmdConnect, cmdStop, cmdUpdate, cmdStart}
	if len(f.cmds) != len(want) {
		t.Fatalf("commands = %v, want %v", f.cmds, want)
	}
	for i := range want {
		if f.cmds[i] != want[i] {
			t.Fatalf("commands = %v, want %v", f.cmds, want)
		}
	}
}

func TestUploadCRCMismatch(t *testing.T) {
	f := &fakeBurnd{badCRC: true}
	c := newTestClient(t, f)
	err := c.Upload(randomFirmware(300))
	if err == nil || !strings.Contains(err.Error(), "verification failed") {
		t.Fatalf("err = %v, want verification failure", err)
	}
	if len(f.cmds) != 4 || f.cmds[3] != cmdUpdate {
		t.Fatalf("START must not be sent after a failed update, got %v", f.cmds)
	}
}

func TestUploadStopFailureIsTolerated(t *testing.T) {
	f := &fakeBurnd{status: map[command]uint16{cmdStop: statusStopErr}}
	c := newTestClient(t, f)
	if err := c.Upload(randomFirmware(100)); err != nil {
		t.Fatal(err)
	}
}

func TestUploadConnectFailure(t *testing.T) {
	f := &fakeBurnd{status: map[command]uint16{cmdConnect: statusOptsErr}}
	c := newTestClient(t, f)
	err := c.Upload(randomFirmware(100))
	if err == nil || !strings.Contains(err.Error(), "connect") {
		t.Fatalf("err = %v, want connect failure", err)
	}
}

func TestUploadNoReply(t *testing.T) {
	f := &fakeBurnd{silent: true}
	c := newTestClient(t, f)
	start := time.Now()
	err := c.Upload(randomFirmware(100))
	if err == nil || !strings.Contains(err.Error(), "not responding") {
		t.Fatalf("err = %v, want not responding", err)
	}
	if elapsed := time.Since(start); elapsed > 2*time.Second {
		t.Fatalf("timeout took %s", elapsed)
	}
}

func TestUploadEmptyFirmware(t *testing.T) {
	c := newTestClient(t, &fakeBurnd{})
	if err := c.Upload(nil); err == nil {
		t.Fatal("empty firmware accepted")
	}
}
