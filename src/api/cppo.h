#pragma once

#include <cppo/types.h>
#ifndef _IOSSTACK_TDEF
#define _IOSSTACK_TDEF
typedef struct iosstack* IOsStack;
#endif

/**
 * @param args @ref
 * @returns @produces child process info
 */
ExeRunResult exe_runa(argsarr args, IOsStack io, bool background);

/**
 * @param cmd @ref
 * @returns @produces child process info
 */
ExeRunResult exe_runs(string_mut cmd, IOsStack io, bool background);

/**
 * @param proc @consumes _on success_
 */
ExeWaitResult exe_waitretcode(ChildProcessInfo proc);

/**
 * @param thread
 */
ExeWaitResult exethread_waitretcode(thread_t thread);
