#include "aliaser.h"

#include <util/null.h>
#include <util/string.h>
#include <cppo/util.h>
#include <ctype.h>

static string_mut whitespaceskip(string_mut s){
	for(; *s && isspace(*s); s++);
	return s;
}

static string_mut whitespacetrim(string_mut str){
	string_mut s = whitespaceskip(str);
	if(!*s) return s;
	string_mut e = s;
	for(; *e; e++);
	for(e--; isspace(*e); e--);
	e[1] = '\0';
	return s;
}

RealiasResult realias(string args[], AliasStore s){
	if(!args || !s) return Error_T(realias_result, {"invalid input"});
	if(!args[0] || !(isalnum(args[0][0]) || args[0][0] == '-' || args[0][0] == '_')) return Ok_T(realias_result, null);
	string_mut repl = aliastore_get(s, args[0]);
	if(!repl) return Ok_T(realias_result, null);
	const bool recurs = *repl && iswspace(repl[strlen(repl)-1]);
	ArgsArr_Mut aargs = exe_args_split(whitespacetrim(repl));
	if(!aargs) return Error_T(realias_result, {"failed to reconstruct arguments"});
	if(recurs){
		const RealiasResult a2 = realias(args+1, s);
		if(!IsOk_T(a2)) retclean(a2, {argsarrmut_destroy(aargs);});
		const ArgsArr_Mut aargs2 = a2.r.ok;
		if(aargs2){
			for(size_t i = 0; i < aargs2->size; i++) if(!IsOk(argsarrmut_append(aargs, aargs->args[i]))) retclean(Error_T(realias_result, {"failed to construct recurse"}), {argsarrmut_destroy(aargs2);argsarrmut_destroy(aargs);}); else aargs->args[i] = null;
			argsarrmut_destroy(aargs2);
		}
	}
	return Ok_T(realias_result, aargs);
}

