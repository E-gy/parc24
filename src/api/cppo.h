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
 * @param args @ref
 * @returns @produces child process info
 */
ExeRunResult exe_run(argsarr args, struct exe_opts opts);

Result_T(exewait_result, int, string_v);
#define ExeWaitResult struct exewait_result

/**
 * @param proc @consumes _on success_
 */
ExeWaitResult exe_waitretcode(ChildProcessInfo proc);

//Utils

/**
 * @param args @ref
 * @returns @produces joined arguments
 */
string_mut exe_args_join(string args[]);
#define exe_args_join_caste(args) exe_args_join((string*)args)

/**
 * @nullable addinfo
 */
void exe_print_last_syserr(string addinfo);
