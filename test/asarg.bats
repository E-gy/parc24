#!/usr/bin/env bats

load common.sh

@test "[arg](1) simply command" {
	run 42test -c "ls"
	[ "$status" -eq 0 ]
	assertout "$(ls)"
}

@test "[arg](1) command with arguments" {
	run 42test -c "ls .."
	[ "$status" -eq 0 ]
	assertout "$(ls ..)"
}

@test "[arg](1) command with arguments and a comment" {
	run 42test -c "ls .. #but not me :P"
	[ "$status" -eq 0 ]
	assertout "$(ls ..)"
}

@test "[arg](1) multiple commands (; delimited)" {
	run 42test -c "ls; ls .."
	[ "$status" -eq 0 ]
	assertout "$(ls; ls ..)"
}

@test "[arg](2) if elif else" {
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; fi' 1
	[ "$status" -eq 0 ]
	assertout "one"
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; fi' 2
	[ "$status" -eq 0 ]
	assertout ""
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi' 2
	[ "$status" -eq 0 ]
	assertout "other"
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi' 2
	[ "$status" -eq 0 ]
	assertout "two"
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi' 3
	[ "$status" -eq 0 ]
	assertout "other"
	run 42test -c 'if [ $1 -eq 1 ] ; then echo -n "one"; elif [ $1 -eq 2 ] ; then echo -n "two"; fi' 3
	[ "$status" -eq 0 ]
	assertout ""
}

@test "[arg](2) for" {
	run 42test -c 'for i do echo -n $i; done'
	[ "$status" -eq 0 ]
	assertout ""
}

@test "[arg](2/3) for args" {
	run 42test -c 'for i do echo -n $i; done' a b c
	[ "$status" -eq 0 ]
	assertout "abc"
}

@test "[arg](2/3) for in" {
	run 42test -c 'for i in d e f; do echo -n $i; done' a b c
	[ "$status" -eq 0 ]
	assertout "def"
}

@test "[arg](2) case" {
	run 42test -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' a
	[ "$status" -eq 0 ]
	assertout "z"
	run 42test -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' b
	[ "$status" -eq 0 ]
	assertout "y"
	run 42test -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' c
	[ "$status" -eq 0 ]
	assertout "x"
	run 42test -c 'case $1 in a ) echo -n z ;; b ) echo -n y ;; c ) echo -n x ;; esac' d
	[ "$status" -eq 0 ]
	assertout ""
}

@test "[arg](3) echo var" {
	run 42test -c 'ABC=hello echo -n $ABC'
	[ "$status" -eq 0 ]
	assertout "hello"
	run 42test -c 'ABC=hello; echo -n $ABC'
	[ "$status" -eq 0 ]
	assertout "hello"
}

@test "[arg](4) for in arith" {
	run 42test -c 'for i in $((1+2)) $((2**3**2)) $((55+6/2)); do echo -n '-' $i; done'
	[ "$status" -eq 0 ]
	assertout "- 3- 512- 58"
}

@test "[arg](4) var, while, echo, arith" {
	run 42test -c 'NUM=0; while [ $NUM -lt 6 ] ; do echo -n "$NUM,"; NUM=$((NUM+2)); done'
	[ "$status" -eq 0 ]
	assertout "0,2,4,"
}

@test "[arg](4) var, until, echo, arith" {
	run 42test -c 'NUM=0; until [ $NUM -gt 6 ] ; do echo -n "$NUM,"; NUM=$((NUM+2)); done'
	[ "$status" -eq 0 ]
	assertout "0,2,4,6,"
}
