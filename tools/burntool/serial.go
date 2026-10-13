package main

import (
	"fmt"

	"go.bug.st/serial"
)

// openSerial opens the board's USB serial port and discards anything left
// in the receive buffer from an earlier session.
func openSerial(name string, baud int) (serial.Port, error) {
	port, err := serial.Open(name, &serial.Mode{BaudRate: baud})
	if err != nil {
		return nil, fmt.Errorf("open %s: %w", name, err)
	}
	if err := port.ResetInputBuffer(); err != nil {
		port.Close()
		return nil, fmt.Errorf("reset %s: %w", name, err)
	}
	return port, nil
}
