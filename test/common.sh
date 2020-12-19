
function memchk(){
	echo "#########################" >> ./memcheck.log
	valgrind --leak-check=full --error-exitcode=100 --log-fd=9 9>>./memcheck.log -- "$@"
	EXC=$?
	echo "#########################" >> ./memcheck.log
	return $EXC
}

function 42test(){
	memchk ./42shcov "$@"
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
