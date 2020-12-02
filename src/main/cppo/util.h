#pragma once

#include <cppo.h>

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
