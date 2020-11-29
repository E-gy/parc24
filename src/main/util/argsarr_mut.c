#include "argsarr_mut.h"

#include "null.h"
#include <stdlib.h>
#include "string.h"

ArgsArr_Mut argsarrmut_new(size_t eargs){
	new(ArgsArr_Mut, a);
	string_mut* args = calloc(eargs+1, sizeof(*args));
	if(!args){
		free(a);
		return null;
	}
	*a = (struct argsarr_mut){eargs, 0, args};
	return a;
}

void argsarrmut_destroy(ArgsArr_Mut a){
	if(!a) return;
	for(size_t i = 0; i < a->size; i++) free(a->args[i]);
	free(a->args);
	free(a);
}

Result argsarrmut_append(ArgsArr_Mut args, string_mut arg){
	if(!args || !arg) return Error;
	if(args->size == args->cap){
		const size_t ncap = args->cap*2;
		string_mut* rargs = realloc(args->args, (ncap+1)*sizeof(*rargs));
		if(!rargs) return Error;
		memset(rargs+args->cap, 0, (ncap+1-args->cap)*sizeof(*rargs));
		args->cap = ncap;
		args->args = rargs;
	}
	args->args[args->size++] = arg;
	return Ok;
}
