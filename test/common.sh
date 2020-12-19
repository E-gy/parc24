
function memchk(){
	echo "#########################" >> ./memcheck.log
	valgrind --leak-check=full --error-exitcode=100 --log-fd=9 9>>./memcheck.log -- "$@"
	EXC=$?
	echo "#########################" >> ./memcheck.log
	return $EXC
}

function 42test(){
	hs=./42sh
	if test -f ./42shcov; then
		hs=./42shcov
	fi
	memchk $hs "$@"
}

function assertsuccess(){
	if [ "$status" -ne 0 ]; then
		echo "actual status: $status"
		echo "command output: $output"
		return 1
	fi
}

function asserteq(){
	if [ "$1" != "$2" ]; then
		echo "expected: $2"
		echo "got: $1"
		return 1
	fi
}

function assertout(){
	asserteq "$output" "$1"
}

function testagainstbash(){
	TMPF=$(mktemp)
	cat - >$TMPF
	run bash "$@" <$TMPF
	expected_o="$output"
	expected_c=$status
	run 42test "$@" <$TMPF
	rm $TMPF
	err=0
	if [ $status -ne $expected_c ]; then
		echo "expected status: $expected_c"
		echo "got status: $status"
		err=1
	fi
	if [ "$output" != "$expected_o" ]; then
		echo "expected output: $expected_o"
		echo "got output: $output"
		err=1
	fi
	return $err
}

function testNeGagainstbash(){
	TMPF=$(mktemp)
	cat - >$TMPF
	run bash "$@" <$TMPF
	expected_o="$output"
	expected_c=$status
	run 42test "$@" <$TMPF
	rm $TMPF
	err=0
	if [ $status -ne $expected_c ]; then
		echo "expected status: $expected_c"
		echo "got status: $status"
		echo "reference output: $expected_o"
		echo "output: $output"
		err=1
	fi
	return $err
}
