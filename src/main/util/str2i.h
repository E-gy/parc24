#pragma once

#include <ptypes.h>

Result_T(str2i_result, long, void*);
#define Str2IResult struct str2i_result

/**
 * @param s @ref 
 * @param radix 
 * @return number 
 */
Str2IResult str2ir(string s, int radix);
#define str2i(s) str2ir(s, 10)
