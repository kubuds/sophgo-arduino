package main

import (
	"bytes"
	"strings"
	"testing"
)

func TestRunFlagErrors(t *testing.T) {
	cases := []struct {
		args []string
		want string
	}{
		{nil, "serial port not set"},
		{[]string{"-p", "/dev/null"}, "firmware file not set"},
		{[]string{"-p", "/dev/null", "-f", "x", "extra"}, "unexpected argument"},
	}
	for _, tc := range cases {
		var out, errOut bytes.Buffer
		if code := run(tc.args, &out, &errOut); code != 2 {
			t.Errorf("run(%v) exit %d, want 2", tc.args, code)
		}
		if !strings.Contains(errOut.String(), tc.want) {
			t.Errorf("run(%v) stderr %q, want %q", tc.args, errOut.String(), tc.want)
		}
	}
}

func TestRunVersionAndHelp(t *testing.T) {
	var out, errOut bytes.Buffer
	if code := run([]string{"--version"}, &out, &errOut); code != 0 || !strings.Contains(out.String(), "burntool") {
		t.Fatalf("--version exit %d, out %q", code, out.String())
	}
	out.Reset()
	if code := run([]string{"-h"}, &out, &errOut); code != 0 || !strings.Contains(errOut.String(), "Usage:") {
		t.Fatalf("-h exit %d, stderr %q", code, errOut.String())
	}
}

func TestRunMissingFirmwareFile(t *testing.T) {
	var out, errOut bytes.Buffer
	if code := run([]string{"-p", "/dev/null", "-f", "/nonexistent.elf"}, &out, &errOut); code != 1 {
		t.Fatalf("exit %d, want 1", code)
	}
}
