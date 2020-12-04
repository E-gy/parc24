#include "str2i.h"

#include "null.h"
#include <stdlib.h>

Str2IResult str2ir(string s, int r){
	if(!s) return Error_T(str2i_result, null);
	string_mut end = null;
	long l = strtol(s, &end, r);
	return end ? Ok_T(str2i_result, l) : Error_T(str2i_result, null);
}
