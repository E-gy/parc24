#include "opts.h"

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

#define tihsopts_default ((struct tihsopts){.parcopts = parc_options_default})

TihsOptsParseResult tihsopts_parse(argsarr args, ParC24IO io){
	if(!args) return Error_T(tihsopts_parse_result, {"Args array invalid"});
	struct tihsopts opts = tihsopts_default;
	for(; *args && strpref("--", *args); args++){
		//TODO long options stuff
	}
	bool readfromstdin = false;
	for(; *args && (strpref("-", *args) || strpref("+", *args)); args++){
		if(streq("-c", *args)){
			if(!*++args) return Error_T(tihsopts_parse_result, {"-c requires an argument"});
			opts.commandstr = *args++;
			break;
		} else if(streq("-s", *args)) readfromstdin = true;
		//TODO more short options
	}
	if(!(opts.commandstr || readfromstdin) && *args) opts.commandfile = *args;
	opts.args = args;
	return Ok_T(tihsopts_parse_result, opts);
}
