#pragma once

#include <cppo.h>
#include <util/argsarr_mut.h>

/**
 * @param args @ref
 * @param ossp OS specific escapes (if the cmd line is to be fed to the execution env provided by the OS)
 * @returns @produces joined arguments
 */
string_mut exe_args_join(string args[], bool ossp);
#define exe_args_join_caste(args, ossp) exe_args_join((string*)args, ossp)

/**
 * @param cmd @ref 
 * @return @produces separated arguments 
 */
ArgsArr_Mut exe_args_split(string cmd);

/**
 * @nullable addinfo
 */
void exe_print_last_syserr(string addinfo);
