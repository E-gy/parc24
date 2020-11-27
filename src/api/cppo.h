#pragma once

#include <ptypes.h>

struct pipe_info {
	fd_t write;
	fd_t read;
};
#define pipe_info struct pipe_info
Result_T(pipe_result, pipe_info, string_v);
#define PipeResult struct pipe_result

PipeResult pipe_new(void);

struct exe_opts {
	/**
	 * Files/handles for the 3 standard streams.
	 * Default -1.
	 */
	fd_t io[3];
};

struct childprocinf;
typedef struct childprocinf* ChildProcessInfo;
Result_T(exerun_result, ChildProcessInfo, string_v);
#define ExeRunResult struct exerun_result

/**
 * @ref args
 * @produces child process info
 */
ExeRunResult exe_run(string* args, struct exe_opts opts);

/**
 * @consumes proc
 */
int exe_waitretcode(ChildProcessInfo proc);
