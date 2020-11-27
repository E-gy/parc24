#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ptypes.h>

#include <cppo.h>

int main(int argc, string_mut args[]){
	if(argc < 2){
		puts("Not enough arguments!");
		return 127;
	}
	ExeRunResult exer = exe_run(args+1, (struct exe_opts){.stdio = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}});
	if(!IsOk_T(exer)){
		puts(exer.r.error.s);
		exe_print_last_syserr("exe_run");
		return 128;
	}
	puts("Child created");
	ExeWaitResult wr = exe_waitretcode(exer.r.ok);
	if(!IsOk_T(wr)){
		puts(wr.r.error.s);
		exe_print_last_syserr("exe_waitretcode");
		return 129;
	}
	puts("Child finished");
	return wr.r.ok;
}
