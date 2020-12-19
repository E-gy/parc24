#!/usr/bin/env bats

load common.sh

@test "[in](1) simply command" {
	testagainstbash <<input
ls
input
}

@test "[in](1) simply commands" {
	testagainstbash <<input
ls
#i am a comment
ls #i am a comment too!
input
}

@test "[in](2) if/elif/else" {
	testagainstbash <<<'if [ 1 -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; elif [ 2 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 3 -eq 1 ] ; then echo -n "one"; elif [ 3 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 3 -eq 1 ] ; then echo -n "one"; elif [ 3 -eq 2 ] ; then echo -n "two"; fi'
}

@test "[in](2) if/elif/else multiline" {
	testagainstbash <<input
if [ 3 -eq 1 ];
	then
			echo -n "one";
	elif [ 3 -eq 2 ] ;
	then echo -n "two";

	#I AM AN ANNOYING LIL' SHIT

else echo -n "other";

	fi
input
}

@test "[in](3) if/elif/else, vars" {
	testagainstbash <<<'i=1; if [ $i -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'i=3; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'i=3; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; fi'
}
