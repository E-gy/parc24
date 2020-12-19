#!/usr/bin/env bats

load common.sh

@test "[in](1) if/elif/else invalid syntax" {
	testNeGagainstbash <<<'if [ 1 -eq 1 ] ; fi'
	testNeGagainstbash <<<'if ; echo hi; fi'
	testNeGagainstbash <<<'if [ 1 -eq 1 ]  then echo -n "one"; fi'
	testNeGagainstbash <<<'if [ 1 -eq 1 ] ; then echo -n "one" fi'
	testNeGagainstbash <<<'if [ 1 -eq 1 ] ; then echo -n "one"; fi'
	testNeGagainstbash <<<'if [ 1 -eq 1 ] ; then echo -n "one";'
	testNeGagainstbash <<<'if [ 1 -eq 1 ] ; echo -n "one"; fi'
	testNeGagainstbash <<<'if [ 3 -eq 1 ] ; then echo -n "one"; elif [ 3 -eq 2 ] ; echo -n "two"; fi'
}
