#pragma once

#include <ptypes.h>
#include <parc24/options.h>

#ifndef _PATTERN_DEF
#define _PATTERN_DEF
typedef struct pattern* Pattern;
#endif

/**
 * @param p @consumes 
 */
void pattern_destroy(Pattern p);

Result_T(patcomp_result, Pattern, string_v);
#define PatCompResult struct patcomp_result

/**
 * @param pat @ref 
 * @param opts @copy
 * @return @produces PatCompResult 
 */
PatCompResult pattern_compile(string pat, struct parc_options opts);

/**
 * Test whether the entire string matches the pattern.
 * 
 * @param p @ref
 * @param str @ref
 */
bool pattern_test(Pattern p, string str);
