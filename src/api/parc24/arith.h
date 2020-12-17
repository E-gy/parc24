#pragma once

#include <ptypes.h>

typedef struct arith_calc* Arithmetics;

/**
 * @return @produces Arithmetics 
 */
Arithmetics arith_new(void);

/**
 * @param arith @consumes 
 */
void arith_destroy(Arithmetics arith);

Result_T(arith_result, double, string_v);
#define ArithResult struct arith_result

/**
 * @param arith @ref 
 * @param expr @ref
 * @return  
 */
ArithResult arith_eval(Arithmetics arith, string expr);
