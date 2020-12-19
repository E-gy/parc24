#!/usr/bin/env bats

load common.sh

@test "say hi with echo" {
	run 42test -c "echo -n hi"
	[ "$status" -eq 0 ]
	[ "$output" = "hi" ]
}
