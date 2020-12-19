#!/usr/bin/env bats

load valgr.sh

@test "say hi with echo" {
	run memchk ./42sh -c "echo -n hi"
	[ "$status" -eq 0 ]
	[ "$output" = "hi" ]
}
