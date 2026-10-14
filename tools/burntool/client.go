package main

import (
	"errors"
	"fmt"
	"hash/crc32"
	"io"
	"os"
	"time"
)

// transport is the byte stream between the host and burnd. A serial port
// satisfies it; tests use an in-memory pipe.
type transport interface {
	io.ReadWriter
	// SetReadTimeout bounds how long a single Read may block.
	SetReadTimeout(d time.Duration) error
}

// Client drives one firmware upload over a transport.
type Client struct {
	T transport

	// ReplyTimeout bounds waiting for the reply to a control command.
	ReplyTimeout time.Duration
	// UpdateTimeout bounds waiting for the board to write and verify the
	// image after the tail frame has been sent. burnd itself gives up
	// after 20 s.
	UpdateTimeout time.Duration
	// TailDelay is the pause between the last firmware byte and the tail
	// frame. burnd reads the port in 64-byte slices and recognises the
	// tail only when it arrives in a read of its own, so the pause lets the
	// board drain everything that is still buffered.
	TailDelay time.Duration
	// ChunkSize is the write size used while streaming the firmware.
	ChunkSize int

	// Progress, when set, is called as firmware bytes are written.
	Progress func(sent, total int)
	// Logf, when set, receives verbose diagnostics.
	Logf func(format string, args ...any)
}

// Default tunables; they mirror the behaviour of the original Python tool.
const (
	defaultReplyTimeout  = 5 * time.Second
	defaultUpdateTimeout = 30 * time.Second
	defaultTailDelay     = 500 * time.Millisecond
	defaultChunkSize     = 1024
)

// NewClient returns a Client with default timeouts.
func NewClient(t transport) *Client {
	return &Client{
		T:             t,
		ReplyTimeout:  defaultReplyTimeout,
		UpdateTimeout: defaultUpdateTimeout,
		TailDelay:     defaultTailDelay,
		ChunkSize:     defaultChunkSize,
	}
}

func (c *Client) logf(format string, args ...any) {
	if c.Logf != nil {
		c.Logf(format, args...)
	}
}

// Upload performs the full sequence: handshake, stop the running firmware,
// stream the new image, verify, restart.
func (c *Client) Upload(firmware []byte) error {
	if len(firmware) == 0 {
		return errors.New("firmware is empty")
	}

	if err := c.mustSucceed(cmdNone, 0); err != nil {
		return fmt.Errorf("board not responding: %w", err)
	}
	if err := c.mustSucceed(cmdConnect, 0); err != nil {
		return fmt.Errorf("connect: %w", err)
	}

	// A STOP failure usually means the firmware was not running; that is
	// not fatal for an upload, so only report it.
	if r, err := c.request(cmdStop, 0, c.ReplyTimeout); err != nil {
		return fmt.Errorf("stop firmware: %w", err)
	} else if !r.OK() {
		c.logf("stop firmware: %s (continuing)", r)
	}

	if err := c.mustSucceed(cmdUpdate, uint32(len(firmware))); err != nil {
		return fmt.Errorf("start update: %w", err)
	}

	if err := c.stream(firmware); err != nil {
		return fmt.Errorf("send firmware: %w", err)
	}

	time.Sleep(c.TailDelay)
	sum := crc32.ChecksumIEEE(firmware)
	c.logf("sending tail, crc32=%08X", sum)
	if _, err := c.T.Write(encodeTail(sum)); err != nil {
		return fmt.Errorf("send tail: %w", err)
	}
	r, err := c.readReply(c.UpdateTimeout)
	if err != nil {
		return fmt.Errorf("waiting for verification: %w", err)
	}
	if r.Cmd != cmdUpdate || !r.OK() {
		return fmt.Errorf("verification failed: %s", r)
	}

	if err := c.mustSucceed(cmdStart, 0); err != nil {
		return fmt.Errorf("start firmware: %w", err)
	}
	return nil
}

// mustSucceed sends cmd and fails unless the board replies OK to it.
func (c *Client) mustSucceed(cmd command, length uint32) error {
	r, err := c.request(cmd, length, c.ReplyTimeout)
	if err != nil {
		return err
	}
	if r.Cmd != cmd {
		return fmt.Errorf("unexpected reply %s to %s", r, cmd)
	}
	if !r.OK() {
		return errors.New(r.String())
	}
	return nil
}

// request sends one command frame and waits for the matching reply.
func (c *Client) request(cmd command, length uint32, timeout time.Duration) (reply, error) {
	c.logf("-> %s length=%d", cmd, length)
	if _, err := c.T.Write(encodeCommand(cmd, length)); err != nil {
		return reply{}, fmt.Errorf("write %s: %w", cmd, err)
	}
	r, err := c.readReply(timeout)
	if err != nil {
		return reply{}, fmt.Errorf("%s: %w", cmd, err)
	}
	c.logf("<- %s", r)
	return r, nil
}

// stream writes the firmware in ChunkSize pieces, reporting progress.
func (c *Client) stream(firmware []byte) error {
	total := len(firmware)
	for sent := 0; sent < total; {
		end := sent + c.ChunkSize
		if end > total {
			end = total
		}
		n, err := c.T.Write(firmware[sent:end])
		if err != nil {
			return err
		}
		sent += n
		if c.Progress != nil {
			c.Progress(sent, total)
		}
	}
	return nil
}

// readReply reads bytes until a whole frame starting with the magic has
// been collected or the deadline passes. Stray bytes before the magic are
// skipped so a stale partial frame cannot desynchronise the exchange.
func (c *Client) readReply(timeout time.Duration) (reply, error) {
	deadline := time.Now().Add(timeout)
	if err := c.T.SetReadTimeout(100 * time.Millisecond); err != nil {
		return reply{}, err
	}

	var frame []byte
	buf := make([]byte, frameSize)
	for {
		n, err := c.T.Read(buf)
		frame = appendFrame(frame, buf[:n])
		if len(frame) == frameSize {
			return decodeReply(frame)
		}
		if err != nil && !isTimeout(err) {
			return reply{}, err
		}
		if time.Now().After(deadline) {
			return reply{}, fmt.Errorf("no reply within %s", timeout)
		}
	}
}

// appendFrame adds data to a partially collected frame, discarding
// anything that precedes the magic bytes.
func appendFrame(frame, data []byte) []byte {
	for _, b := range data {
		switch len(frame) {
		case 0:
			if b == 0xAA {
				frame = append(frame, b)
			}
		case 1:
			if b == 0x55 {
				frame = append(frame, b)
			} else if b != 0xAA {
				frame = frame[:0]
			}
		default:
			frame = append(frame, b)
			if len(frame) == frameSize {
				return frame
			}
		}
	}
	return frame
}

// isTimeout reports whether err only signals an expired read deadline.
func isTimeout(err error) bool {
	if errors.Is(err, os.ErrDeadlineExceeded) {
		return true
	}
	var t interface{ Timeout() bool }
	return errors.As(err, &t) && t.Timeout()
}
