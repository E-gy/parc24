#include "opts.h"

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

#define tihsopts_default ((struct tihsopts){.parcopts = parc_options_default})

#define ifsetopt(opt) (streq(#opt, *args)) opts.parcopts.opt = setto
#define printopt(opt) do { if(printshopt == 1) io.log(LL_INFO, "shopt %s %s", opts.parcopts.opt ? "-s" : "-u", #opt); else if(printshopt == 2) io.log(LL_INFO, "%s	%s", #opt, opts.parcopts.opt ? "on" : "off"); } while(0)

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
			if(!(printshopt = !*++args ? 1+!!setto : 0)){
				if ifsetopt(dotglob);
				else if ifsetopt(extglob);
				else if ifsetopt(nocaseglob);
				else if ifsetopt(nullglob);
				else if ifsetopt(expand_aliases);
				else if ifsetopt(sourcepath);
				else if ifsetopt(xpg_echo);
				else io.log(LL_ERROR, "%s: invalid shell option name", *args);
			}
		} else io.log(LL_ERROR, "%s: invalid option", *args);
	}
	if(printshopt){
		printopt(dotglob);
		printopt(extglob);
		printopt(nocaseglob);
		printopt(nullglob);
		printopt(expand_aliases);
		printopt(sourcepath);
		printopt(xpg_echo);
	}
	if(!(opts.commandstr || readfromstdin) && *args) opts.commandfile = *args;
	opts.args = args;
	return Ok_T(tihsopts_parse_result, opts);
}
