#pragma once

#include <ptypes.h>

struct argsarr_mut {
	size_t cap;
	size_t size;
	string_mut* args;
};
typedef struct argsarr_mut* ArgsArr_Mut;

/**
 * @param eargs expected number of arguments
 * @return @produces arr
 */
ArgsArr_Mut argsarrmut_new(size_t eargs);

/**
 * @param args @consumes
 */
void argsarrmut_destroy(ArgsArr_Mut args);

/**
 * @param args @refmut arguments
 * @param arg @consumes argument to add
 * @return Result 
 */
Result argsarrmut_append(ArgsArr_Mut args, string_mut arg);
