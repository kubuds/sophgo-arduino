package main

import (
	"encoding/binary"
	"errors"
	"fmt"
)

// Wire protocol spoken by the burnd daemon running on the board.
//
// Every control frame is exactly 8 bytes and starts with the magic bytes
// 0xAA 0x55. All multi-byte integers are little-endian.
//
//	command : AA 55 | cmd u16 | length u32
//	reply   : AA 55 | cmd u16 | status u16 | errno u16
//	tail    : crc32 u32 | FF FF FF FF
//
// A firmware update is: UPDATE(length) -> raw firmware bytes -> tail.
// The board answers the tail with an UPDATE reply once the image has been
// written and its CRC32 verified.

const (
	frameSize  = 8
	frameMagic = 0xAA55

	frameTailMarker = 0xFFFFFFFF
)

// command identifies a request sent to burnd (enum TASK_CMD).
type command uint16

const (
	cmdNone command = iota
	cmdConnect
	cmdStop
	cmdStart
	cmdUpdate
	cmdError // sent back by burnd for an unknown command
)

func (c command) String() string {
	switch c {
	case cmdNone:
		return "NONE"
	case cmdConnect:
		return "CONNECT"
	case cmdStop:
		return "STOP"
	case cmdStart:
		return "START"
	case cmdUpdate:
		return "UPDATE"
	case cmdError:
		return "ERROR"
	}
	return fmt.Sprintf("CMD(%d)", uint16(c))
}

// Status codes reported by burnd in the reply frame (enum TASK_ERRNO).
const (
	statusOK uint16 = iota
	statusTimeout
	statusStartErr
	statusStopErr
	statusOptsErr
	statusSchedErr
	statusCheckErr
	statusDataErr
)

func statusString(s uint16) string {
	switch s {
	case statusOK:
		return "ok"
	case statusTimeout:
		return "timeout on board"
	case statusStartErr:
		return "start failed"
	case statusStopErr:
		return "stop failed"
	case statusOptsErr:
		return "sysfs/remoteproc operation failed"
	case statusSchedErr:
		return "task scheduling failed"
	case statusCheckErr:
		return "check failed"
	case statusDataErr:
		return "firmware size or CRC32 mismatch"
	}
	return fmt.Sprintf("status %d", s)
}

// reply is a decoded reply frame.
type reply struct {
	Cmd    command
	Status uint16
	Errno  uint16
}

// OK reports whether the board accepted the command.
func (r reply) OK() bool { return r.Status == statusOK }

func (r reply) String() string {
	return fmt.Sprintf("%s: %s (errno %d)", r.Cmd, statusString(r.Status), r.Errno)
}

var errBadMagic = errors.New("frame does not start with AA 55")

// encodeCommand builds a command frame.
func encodeCommand(cmd command, length uint32) []byte {
	b := make([]byte, frameSize)
	b[0], b[1] = 0xAA, 0x55
	binary.LittleEndian.PutUint16(b[2:], uint16(cmd))
	binary.LittleEndian.PutUint32(b[4:], length)
	return b
}

// encodeTail builds the frame that terminates a firmware transfer.
func encodeTail(crc uint32) []byte {
	b := make([]byte, frameSize)
	binary.LittleEndian.PutUint32(b[0:], crc)
	binary.LittleEndian.PutUint32(b[4:], frameTailMarker)
	return b
}

// decodeReply parses an 8-byte reply frame.
func decodeReply(b []byte) (reply, error) {
	if len(b) != frameSize {
		return reply{}, fmt.Errorf("reply frame is %d bytes, want %d", len(b), frameSize)
	}
	if b[0] != 0xAA || b[1] != 0x55 {
		return reply{}, errBadMagic
	}
	return reply{
		Cmd:    command(binary.LittleEndian.Uint16(b[2:])),
		Status: binary.LittleEndian.Uint16(b[4:]),
		Errno:  binary.LittleEndian.Uint16(b[6:]),
	}, nil
}
