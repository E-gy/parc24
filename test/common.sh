function memchk(){
	echo "#########################" >> ./memchck.log
	valgrind --leak-check=full --error-exitcode=100 --log-fd=9 9>>./memchck.log -- "$@"
	EXC=$?
	echo "#########################" >> ./memchck.log
	return $EXC
}

function 42test(){
	memchk ./42shcov "$@"
}
