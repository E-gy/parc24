#pragma once

#include <cppo/types.h>

/**
 * @param args @ref
 * @returns @produces child process info
 */
ExeRunResult exe_runa(argsarr args, struct exe_opts opts);

/**
 * @param cmd @ref
 * @returns @produces child process info
 */
ExeRunResult exe_runs(string_mut cmd, struct exe_opts opts);

/**
 * @param proc @consumes _on success_
 */
ExeWaitResult exe_waitretcode(ChildProcessInfo proc);

/**
 * @param thread
 */
ExeWaitResult exethread_waitretcode(thread_t thread);
