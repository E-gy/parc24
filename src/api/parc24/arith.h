#pragma once

#include <ptypes.h>

#ifndef _ARITH_DEF
#define _ARITH_DEF
typedef struct arith_calc* Arithmetics;
#endif

/**
 * @return @produces Arithmetics 
 */
Arithmetics arith_new(void);

/**
 * @param arith @consumes 
 */
void arith_destroy(Arithmetics arith);

typedef long long arithnum;
Result_T(arith_result, arithnum, string_v);
#define ArithResult struct arith_result

/**
 * @param arith @ref 
 * @param expr @ref
 * @return  
 */
ArithResult arith_eval(Arithmetics arith, string expr);
