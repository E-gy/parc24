#include "aliaser.h"

#include <util/null.h>
#include <util/string.h>
#include <cppo/util.h>
#include <ctype.h>

static string_mut whitespaceskip(string_mut s){
	for(; *s && isspace(*s); s++);
	return s;
}

typedef struct realisstek* RealiaStek;
struct realisstek {
	string av;
	RealiaStek next;
};

static RealiasResult realias_(string args[], AliasStore s, RealiaStek stek){
	if(!args || !s) return Error_T(realias_result, {"invalid input"});
	if(!args[0] || !(isalnum(args[0][0]) || args[0][0] == '-' || args[0][0] == '_')) return Ok_T(realias_result, null);
	string_mut repl = aliastore_get(s, args[0]);
	if(!repl) return Ok_T(realias_result, null);
	for(RealiaStek k = stek; k; k = k->next) if(streq(args[0], k->av)) return Ok_T(realias_result, null);;
	const bool recurs = *repl && isspace(repl[strlen(repl)-1]);
	ArgsArr_Mut aargs = exe_args_split(whitespaceskip(repl));
	if(!aargs) return Error_T(realias_result, {"failed to reconstruct arguments"});
	if(recurs){
		struct realisstek snext = {args[0], stek};
		const RealiasResult a2 = realias_(args+1, s, &snext);
		if(!IsOk_T(a2)) retclean(a2, {argsarrmut_destroy(aargs);});
		const ArgsArr_Mut aargs2 = a2.r.ok;
		if(aargs2){
			for(size_t i = 0; i < aargs2->size; i++) if(!IsOk(argsarrmut_append(aargs, aargs2->args[i]))) retclean(Error_T(realias_result, {"failed to construct recurse"}), {argsarrmut_destroy(aargs2);argsarrmut_destroy(aargs);}); else aargs2->args[i] = null;
			argsarrmut_destroy(aargs2);
		}
	} else for(string* s = args+1; *s; s++){
		string_mut dup = strdup(*s);
		if(!dup || !IsOk(argsarrmut_append(aargs, dup))) retclean(Error_T(realias_result, {"failed to reconstruct args"}), {argsarrmut_destroy(aargs);});
	}
	return Ok_T(realias_result, aargs);
}

RealiasResult realias(string args[], AliasStore s){
	return realias_(args, s, null);
}
