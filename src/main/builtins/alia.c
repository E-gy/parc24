#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <parc24/ioslog.h>

TraverseASTResult cmd_alias(argsarr args, ParContext context){
	size_t argc = 0;
	for(argsarr a = args; *a; a++) argc++;
	bool print = argc < 2 || streq("-p", args[1]);
	for(size_t i = 1; i < argc; i++){
		if(i == 1 && streq("-p", args[i])) continue;
		string_mut naa = args[i];
		string_mut sep = strchr(naa, '=');
		if(!sep){
			parciolog(context->ios, LL_ERROR, "not an alias specifier: %s", naa);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
		}
		*sep = '\0';
		Result aa = aliastore_add(context->aliases, naa, sep+1);
		*sep = '=';
		if(!IsOk(aa)) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	}
	if(print){
		//TODO
	}
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
}

TraverseASTResult cmd_unalias(argsarr args, ParContext context){
	size_t argc = 0;
	for(argsarr a = args; *a; a++) argc++;
	if(argc > 1 && streq("-a", args[1])); //TODO remove all
	else for(size_t i = 1; i < argc; i++) if(!IsOk(aliastore_remove(context->aliases, args[i]))) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
}
