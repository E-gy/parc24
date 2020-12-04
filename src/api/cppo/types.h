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

#define IOSTREAM_STD_IN 0
#define IOSTREAM_STD_OUT 1
#define IOSTREAM_STD_ERR 2

struct exe_opts {
	/**
	 * Files/handles the standard streams.
	 * Default -1.
	 */
	fd_t iostreams[10];
	/**
	 * if true, detaches all handles to the child (so the returned info _will_ be null) making it unawaitable 
	 */
	bool background;
};

struct childprocinf;
typedef struct childprocinf* ChildProcessInfo;
Result_T(exerun_result, ChildProcessInfo, string_v);
#define ExeRunResult struct exerun_result

Result_T(exewait_result, int, string_v);
#define ExeWaitResult struct exewait_result

