#include "opts.h"

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

#define tihsopts_default ((struct tihsopts){.parcopts = parc_options_default})

#define printopt(opt, reusable) do { if(printshopt == 1) io.logger(LL_INFO, "shopt %s %s", opts.parcopts. ## opt ? "-s" : "-u", #opt); else if(printshopt == 2) io.logger(LL_INFO, "%s	%s", #opt, opts.parcopts. ## opt ? "on" : "off"); } while(0)

TihsOptsParseResult tihsopts_parse(argsarr args, ParC24IO io){
	if(!args) return Error_T(tihsopts_parse_result, {"Args array invalid"});
	struct tihsopts opts = tihsopts_default;
	for(; *args && strpref("--", *args); args++){
		//TODO long options stuff
	}
	bool readfromstdin = false;
	int printshopt = 0;
	for(; *args && (strpref("-", *args) || strpref("+", *args)); args++){
		if(streq("-c", *args)){
			if(!*++args) return Error_T(tihsopts_parse_result, {"-c requires an argument"});
			opts.commandstr = *args++;
			break;
		} else if(streq("-s", *args)) readfromstdin = true;
		else if(streq("-O", *args) || streq("+O", *args)){
			bool setto = **args == '+' ? false : true;
			printshopt = !*++args ? 1+!!setto : 0;
		}
	}
	if(printshopt){
		printopt(dotglob, setto);
		printopt(extglob, setto);
		printopt(nocaseglob, setto);
		printopt(nullglob, setto);
		printopt(expand_aliases, setto);
		printopt(sourcepath, setto);
		printopt(xpg_echo, setto);
	}
	if(!(opts.commandstr || readfromstdin) && *args) opts.commandfile = *args;
	opts.args = args;
	return Ok_T(tihsopts_parse_result, opts);
}
