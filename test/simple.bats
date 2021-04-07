#!/usr/bin/env bats

load common.sh

@test "say hi with echo" {
	run test24 -c "echo -n hi"
	[ "$status" -eq 0 ]
	[ "$output" = "hi" ]
}
