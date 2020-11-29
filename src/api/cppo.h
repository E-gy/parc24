#pragma once

#include <cppo/types.h>

/**
 * @param args @ref
 * @returns @produces child process info
 */
ExeRunResult exe_run(argsarr args, struct exe_opts opts);

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
