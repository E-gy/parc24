#pragma once

#include <cppo.h>
#include <util/argsarr_mut.h>

/**
 * @param args @ref
 * @returns @produces joined arguments
 */
string_mut exe_args_join(string args[]);
#define exe_args_join_caste(args) exe_args_join((string*)args)

/**
 * @param cmd @ref 
 * @return @produces separated arguments 
 */
ArgsArr_Mut exe_args_split(string cmd);

/**
 * @nullable addinfo
 */
void exe_print_last_syserr(string addinfo);
