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
	struct { fd_t in, out, err; } stdio;
};

struct childprocinf;
typedef struct childprocinf* ChildProcessInfo;
Result_T(exerun_result, ChildProcessInfo, string_v);
#define ExeRunResult struct exerun_result

/**
 * @ref args
 * @produces child process info
 */
ExeRunResult exe_run(char* const* args, struct exe_opts opts);

Result_T(exewait_result, int, string_v);
#define ExeWaitResult struct exewait_result

/**
 * @consumes proc _on success_
 */
ExeWaitResult exe_waitretcode(ChildProcessInfo proc);

/**
 * @produces error description
 */
string_v get_last_error_desc(void);
