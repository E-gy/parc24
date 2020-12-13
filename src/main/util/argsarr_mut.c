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

static Result argsarrmut_capset(ArgsArr_Mut args, size_t ncap){
	if(!args) return Error;
	if(ncap <= args->cap) return Ok;
	string_mut* rargs = realloc(args->args, (ncap+1)*sizeof(*rargs));
	if(!rargs) return Error;
	memset(rargs+args->cap, 0, (ncap+1-args->cap)*sizeof(*rargs));
	args->cap = ncap;
	args->args = rargs;
	return Ok;
}

Result argsarrmut_append(ArgsArr_Mut args, string_mut arg){
	if(!args || !arg) return Error;
	if(args->size == args->cap) if(!IsOk(argsarrmut_capset(args, args->cap*2))) return Error;
	args->args[args->size++] = arg;
	return Ok;
}

Result argsarrmut_appendnom(ArgsArr_Mut args, ArgsArr_Mut add){
	if(!args || !add) return Error;
	if(args->size + add->size > args->cap) if(!IsOk(argsarrmut_capset(args, args->size + add->size))) return Error;
	for(size_t i = 0; i < add->size; i++) args->args[args->size++] = add->args[i];
	free(add->args);
	free(add);
	return Ok;
}

ArgsArr_Mut argsarrmut_from(string args[]){
	if(!args) return null;
	size_t c = 0;
	for(; args[c]; c++);
	ArgsArr_Mut a = argsarrmut_new(c);
	if(!a) return null;
	for(size_t i = 0; i < c; i++){
		string_mut strd = strdup(args[i]);
		if(!strd) retclean(null, {argsarrmut_destroy(a);});
		argsarrmut_append(a, strd);
	}
	return a;
}
