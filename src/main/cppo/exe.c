#include <cppo.h>
#include "util.h"

#include <stdlib.h>
#include <util/null.h>

#define cpr_new(var) ChildProcessInfo var = opts.background ? null : malloc(sizeof(*var)); if(!var && !opts.background) return Error_T(exerun_result, {"Process info allocation failed"})

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

static Result fd2handle(fd_t io, fd_t stream, STARTUPINFO* inf){
	if(io != IOSTREAM_STD_IN && io != IOSTREAM_STD_OUT && io != IOSTREAM_STD_ERR) return Ok; //FIXME support more streams on Win?
	intptr_t _h = _get_osfhandle(stream);
	if(_h < 0) return Error;
	if(!SetHandleInformation((HANDLE) _h, HANDLE_FLAG_INHERIT, true)) return Error;
	*(io == IOSTREAM_STD_IN ? &inf->hStdInput : io == IOSTREAM_STD_OUT ? &inf->hStdOutput : &inf->hStdError) = (HANDLE) _h;
	return Ok;
}
static Result fd2handle_wrap(fd_t io, fd_t stream, void* inf){
	return fd2handle(io, stream, inf);
}

ExeRunResult exe_runa(argsarr args, struct exe_opts opts){
	string_mut cmd = exe_args_join_caste(args, true);
	if(!cmd) return Error_T(exerun_result, {"Args join failed"});
	ExeRunResult rr = exe_runs(cmd, opts);
	free(cmd);
	return rr;
}

ExeRunResult exe_runs(string_mut cmd, struct exe_opts opts){
	STARTUPINFO startup = {.dwFlags = STARTF_USESTDHANDLES};
	if(!IsOk(iosstack_foreach(opts.iostreams, fd2handle_wrap, &startup))) return Error_T(exerun_result, {"Passing handles to child failed"});
	cpr_new(procinf);
	PROCESS_INFORMATION cpi;
	bool ok = CreateProcessA(null, cmd, null, null, true, 0, null, null, &startup, &cpi);
	if(!ok) return Error_T(exerun_result, {"CreateProcess failed :("});
	if(opts.background) CloseHandle(cpi.hProcess);
	else procinf->ph = cpi.hProcess;
	CloseHandle(cpi.hThread);
	return Ok_T(exerun_result, procinf);
}

ExeWaitResult exe_waitretcode(ChildProcessInfo proc){
	if(!proc) return Error_T(exewait_result, {"Invalid child process info"});
	if(WaitForSingleObject(proc->ph, INFINITE) != 0) return Error_T(exewait_result, {"Wait failed"});
	DWORD exitcode;
	if(!GetExitCodeProcess(proc->ph, &exitcode)) return Error_T(exewait_result, {"GetExitCode failed"});
	CloseHandle(proc->ph);
	free(proc);
	return Ok_T(exewait_result, exitcode);
}

#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

struct childprocinf {
	pid_t pid;
};

static Result fdremainopenonexec(fd_t fd){
	int flags = fcntl(fd, F_GETFD, 0);
	if(flags < 0) return Error;
	flags &= ~FD_CLOEXEC;
	return fcntl(fd, F_SETFD, flags) < 0 ? Error : Ok;
}

static Result fdup(fd_t io, fd_t stream, void* inf){
	if(dup2(stream, io) < 0) return Error;
	if(!IsOk(fdremainopenonexec(io))) return Error;
	return Ok;
}

#define fvoid(fdst) do { fd_t _devnul = open("/dev/null", O_RDWR); if(_devnul < 0) return Error_T(exerun_result, {"open /dev/null failed"}); if(dup2(_devnul, fdst) < 0) return Error_T(exerun_result, {"dup2 failed"}); close(_devnul); } while(0)

ExeRunResult exe_runa(argsarr args, struct exe_opts opts){
	cpr_new(procinf);
	pid_t cpid = fork();
	if(cpid < 0){
		free(procinf);
		return Error_T(exerun_result, {"fork failed"});
	}
	if(cpid == 0){
		if(opts.background) if(fork() != 0) exit(69);
		iosstack_foreach(opts.iostreams, fdup, null);
		if(!iosstack_raw_has(opts.iostreams, IOSTREAM_STD_IN)) fvoid(STDIN_FILENO);
		if(!iosstack_raw_has(opts.iostreams, IOSTREAM_STD_OUT)) fvoid(STDOUT_FILENO);
		if(!iosstack_raw_has(opts.iostreams, IOSTREAM_STD_ERR)) fvoid(STDERR_FILENO);
		execvp(args[0], args);
		exit(69); //exec failed
	}	
	if(opts.background){
		int cstatus;
		if(waitpid(cpid, &cstatus, 0) < 0) return Error_T(exerun_result, {"wait failed"});
	} else procinf->pid = cpid;
	return Ok_T(exerun_result, procinf);
}

ExeRunResult exe_runs(string_mut cmd, struct exe_opts opts){
	ArgsArr_Mut args = exe_args_split(cmd);
	if(!args) return Error_T(exerun_result, {"Args new failed"});
	ExeRunResult rr = exe_runa(args->args, opts);
	argsarrmut_destroy(args);
	return rr;
}

ExeWaitResult exe_waitretcode(ChildProcessInfo proc){
	if(!proc) return Error_T(exewait_result, {"Invalid child process info"});
	int cstatus;
	if(waitpid(proc->pid, &cstatus, 0) < 0) return Error_T(exewait_result, {"wait failed"});
	return Ok_T(exewait_result, WEXITSTATUS(cstatus));
}

#endif
