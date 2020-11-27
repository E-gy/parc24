#include <cppo.h>

#include <stdlib.h>
#include <util/null.h>

#define cpr_new(var) ChildProcessInfo var = malloc(sizeof(*var)); if(!var) return Error_T(exerun_result, {"Process info allocation failed"})

#ifdef _WIN32

#include <handleapi.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <io.h>
// Hackity hackity
// #include <winbase.h>
#ifndef STARTF_USESTDHANDLES
#define STARTF_USESTDHANDLES 0x00000100
#endif // STARTF_USESTDHANDLES
#ifndef HANDLE_FLAG_INHERIT
#define HANDLE_FLAG_INHERIT 0x1
#endif // HANDLE_FLAG_INHERIT
#ifndef INFINITE
#define INFINITE 0xffffffff
#endif // INFINITE

struct childprocinf {
	HANDLE ph;
};

#define fd2handle(fd, handle) do { intptr_t _h = _get_osfhandle(fd); if(_h < 0) return Error_T(exerun_result, {"File not associated with a stream - can't retrieve handle"}); if(!SetHandleInformation((HANDLE) _h, HANDLE_FLAG_INHERIT, true)) return Error_T(exerun_result, {"Handle set inherit failed"}); handle = (HANDLE) _h; } while(0)

ExeRunResult exe_run(string_mut args, struct exe_opts opts){
	STARTUPINFO startup = {.dwFlags = STARTF_USESTDHANDLES};
	if(opts.stdio.in >= 0) fd2handle(opts.stdio.in, startup.hStdInput);
	if(opts.stdio.out >= 0) fd2handle(opts.stdio.out, startup.hStdOutput);
	if(opts.stdio.err >= 0) fd2handle(opts.stdio.err, startup.hStdError);
	cpr_new(procinf);
	PROCESS_INFORMATION cpi;
	if(!CreateProcessA(null, args, null, null, true, 0, null, null, &startup, &cpi)) return Error_T(exerun_result, {"CreateProcess failed :("});
	procinf->ph = cpi.hProcess;
	CloseHandle(cpi.hThread);
	return Ok_T(exerun_result, procinf);
}

ExeWaitResult exe_waitretcode(ChildProcessInfo proc){
	if(WaitForSingleObject(proc->ph, INFINITE) != 0) return Error_T(exewait_result, {"Wait failed"});
	DWORD exitcode;
	if(!GetExitCodeProcess(proc->ph, &exitcode)) return Error_T(exewait_result, {"GetExitCode failed"});
	free(proc);
	return Ok_T(exewait_result, exitcode);
}

#else

struct childprocinf {
	pid_t pid;
};

ExeRunResult exe_run(string* args, struct exe_opts opts){
	cpr_new(procinf);
	//TODO
	return Ok_T(exerun_result, procinf);
}

int exe_waitretcode(ChildProcessInfo proc); //TODO

#endif
