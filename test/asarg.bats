#!/usr/bin/env bats

load common.sh

@test "[arg](1) simply command" {
	run test24 -c "ls"
	assertsuccess
	assertout "$(ls)"
}

@test "[arg](1) command with arguments" {
	run test24 -c "ls .."
	assertsuccess
	assertout "$(ls ..)"
}

@test "[arg](1) command with arguments and a comment" {
	run test24 -c "ls .. #but not me :P"
	assertsuccess
	assertout "$(ls ..)"
}

@test "[arg](1) multiple commands (; delimited)" {
	run test24 -c "ls; ls .."
	assertsuccess
	assertout "$(ls; ls ..)"
}

@test "[arg](2) if elif else" {
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; fi' 1
	assertsuccess
	assertout "one"
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; fi' 2
	assertsuccess
	assertout ""
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi' 2
	assertsuccess
	assertout "other"
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi' 2
	assertsuccess
	assertout "two"
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi' 3
	assertsuccess
	assertout "other"
	run test24 -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; fi' 3
	assertsuccess
	assertout ""
}

@test "[arg](2) for" {
	run test24 -c 'for i do echo -n $i; done'
	assertsuccess
	assertout ""
}

@test "[arg](2/3) for args" {
	run test24 -c 'for i do echo -n $i; done' a b c
	assertsuccess
	assertout "abc"
}

@test "[arg](2/3) for in" {
	run test24 -c 'for i in d e f; do echo -n $i; done' a b c
	assertsuccess
	assertout "def"
}

@test "[arg](2) case" {
	run test24 -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' a
	assertsuccess
	assertout "z"
	run test24 -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' b
	assertsuccess
	assertout "y"
	run test24 -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' c
	assertsuccess
	assertout "x"
	run test24 -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' d
	assertsuccess
	assertout ""
}

@test "[arg](3) echo var" {
	run test24 -c 'ABC=hello echo -n $ABC'
	assertsuccess
	assertout "hello"
	run test24 -c 'ABC=hello; echo -n $ABC'
	assertsuccess
	assertout "hello"
}

@test "[arg](4) for in arith" {
	run test24 -c 'for i in $((1+2)) $((2**3**2)) $((55+6/2)); do echo -n '-' $i; done'
	assertsuccess
	assertout "- 3- 512- 58"
}

@test "[arg](4) var, while, echo, arith" {
	run test24 -c 'NUM=0; while [ $NUM -lt 6 ] ; do echo -n "$NUM,"; NUM=$((NUM+2)); done'
	assertsuccess
	assertout "0,2,4,"
}

@test "[arg](4) var, until, echo, arith" {
	run test24 -c 'NUM=0; until [ $NUM -gt 6 ] ; do echo -n "$NUM,"; NUM=$((NUM+2)); done'
	assertsuccess
	assertout "0,2,4,6,"
}

@test "[arg](4) nested arith & loops" {
	run test24 -c 'echo -n $((123+$(for i in 3 2 1 ; do echo -n $((i+$(until [ $i -gt 2 ] ; do i=$((i*2)) ; done ; echo -n $i ))) ; done)))'
	assertsuccess
	assertout "788"
}
