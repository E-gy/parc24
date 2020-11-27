#include <cppo.h>

#ifdef _WIN32

#include <handleapi.h>
#include <namedpipeapi.h>
#include <fcntl.h>

PipeResult pipe_new(void){
	HANDLE read;
	HANDLE write;
	if(!CreatePipe(&read, &write, NULL, 0)) return Error_T(pipe_result, {"Pipe creation failed"});
	fd_t readf = _open_osfhandle((intptr_t) read, 0);
	fd_t writef = _open_osfhandle((intptr_t) write, 0);
	if(readf < 0 || writef < 0){
		if(readf >= 0) close(readf); else CloseHandle(read);
		if(writef >= 0) close(writef); else CloseHandle(write);
		return Error_T(pipe_result, {"Handle to file conversion failed"});
	}
	return Ok_T(pipe_result, {.write = writef, .read = readf});
}

#else

#include <unistd.h>
#include <fcntl.h>

PipeResult pipe_new(void){
	fd_t piped[2];
	if(pipe2(piped, O_CLOEXEC) < 0) return Error_T(pipe_result, {"Pipe creation failed"});
	return Ok_T(pipe_result, {.write = piped[1], .read = piped[0]});
}

#endif
