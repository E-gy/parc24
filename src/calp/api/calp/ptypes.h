#pragma once

/**
 * @file ptypes.h
 * Primitive Types commonly used by CALP.
 * 
 * And some operations for some of them.
 * 
 */

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>

typedef bool Result;
#define Ok false
#define Error true
#define IsOk(result) (result == Ok)
#define IsNotOk(result) !IsOk(result)

#define Result_T(T, okt, errt) struct T { Result result; union { okt ok; errt error; } r; } 
#define Ok_T(T,...) ((struct T){Ok, {.ok=__VA_ARGS__}})
#define Error_T(T,...) ((struct T){Error, {.error=__VA_ARGS__}})
#define IsOk_T(resultt) IsOk(resultt.result)
#define IsNotOk_T(resultt) IsNotOk(resultt.result)
#define IfOk_T(result, var, closure) do{ typeof(result) _r = result; if(IsOk_T(_r)){ typeof(_r.r.ok) var = _r.r.ok; closure } }while(0)
#define IfError_T(result, var, closure) do{ typeof(result) _r = result; if(IsNotOk_T(_r)){ typeof(_r.r.error) var = _r.r.error; closure } }while(0)
#define IfElse_T(result, vok, clok, verr, clerr) do{ typeof(result) _r = result; if(IsOk_T(_r)){ typeof(_r.r.ok) vok = _r.r.ok; clok } else { typeof(_r.r.error) verr = _r.r.error; clerr } }while(0)
#define OrElse_T(result, els) __extension__({ typeof(result) _r = result; IsOk_T(result) ? result.r.ok : els; })

typedef const char* string;
typedef char* string_mut;
struct string_v {
	char s[128];
};
#define string_v struct string_v
