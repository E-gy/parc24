#include "opts.h"

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>
#include <builtins/ccmds.h>
#include <getopt.h>

#define tihsopts_default ((struct tihsopts){.parcopts = parc_options_default})

#define printopt(opts, opt, io) do { if(printshopt == 1) io.log(LL_INFO, "shopt %s %s", (opts).opt ? "-s" : "-u", #opt); else if(printshopt == 2) io.log(LL_INFO, "%s	%s", #opt, (opts).opt ? "on" : "off"); } while(0)

#define switchsetopt(opts, name, v, unknoclo) do { \
		if(streq("dotglob", name)) (opts).dotglob = v; \
		else if(streq("extglob", name)) (opts).extglob = v; \
		else if(streq("nocaseglob", name)) (opts).nocaseglob = v; \
		else if(streq("nullglob", name)) (opts).nullglob = v; \
		else if(streq("expand_aliases", name)) (opts).expand_aliases = v; \
		else if(streq("sourcepath", name)) (opts).sourcepath = v; \
		else if(streq("xpg_echo", name)) (opts).xpg_echo = v; \
		else unknoclo \
	} while(0)

#define ntarrcontainsopt(arr, opt) __extension__({ bool _c = false; for(__typeof__ (arr) _a = (arr); !_c && *_a; _a++) if(streq(opt, *_a)) _c = true; _c; })

#define printopts(opts, sel, io, only) do { \
		if(((sel) == 0 || ((opts).dotglob == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "dotglob"))) printopt(opts, dotglob, io); \
		if(((sel) == 0 || ((opts).extglob == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "extglob"))) printopt(opts, extglob, io); \
		if(((sel) == 0 || ((opts).nocaseglob == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "nocaseglob"))) printopt(opts, nocaseglob, io); \
		if(((sel) == 0 || ((opts).nullglob == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "nullglob"))) printopt(opts, nullglob, io); \
		if(((sel) == 0 || ((opts).expand_aliases == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "expand_aliases"))) printopt(opts, expand_aliases, io); \
		if(((sel) == 0 || ((opts).sourcepath == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "sourcepath"))) printopt(opts, sourcepath, io); \
		if(((sel) == 0 || ((opts).xpg_echo == ((sel) == 1))) && (!(only) || ntarrcontainsopt(only, "xpg_echo"))) printopt(opts, xpg_echo, io); \
	} while(0)

#define optsallenabled(opts, only) (\
		((opts).dotglob || !ntarrcontainsopt(only, "dotglob")) && \
		((opts).extglob || !ntarrcontainsopt(only, "extglob")) && \
		((opts).nocaseglob || !ntarrcontainsopt(only, "nocaseglob")) && \
		((opts).nullglob || !ntarrcontainsopt(only, "nullglob")) && \
		((opts).expand_aliases || !ntarrcontainsopt(only, "expand_aliases")) && \
		((opts).sourcepath || !ntarrcontainsopt(only, "sourcepath")) && \
		((opts).xpg_echo || !ntarrcontainsopt(only, "xpg_echo")) \
	)

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
			if(!(printshopt = !*++args ? 1+!!setto : 0)) switchsetopt(opts.parcopts, *args, setto, { io.log(LL_ERROR, "shopt: %s: invalid shell option name", *args); });
			else args--;
		} else io.log(LL_ERROR, "%s: invalid option", *args);
	}
	if(printshopt) printopts(opts.parcopts, 0, io, false ? args : null);
	if(!(opts.commandstr || readfromstdin) && *args) opts.commandfile = *args;
	opts.args = args;
	return Ok_T(tihsopts_parse_result, opts);
}

TraverseASTResult cmd_shopt(argsarr args, ParContext ctxt){
	//output is smol if any so no parallels required
	//FIXME output to pipe - that said, piping from shopt will not work as of currently :/
	const size_t argc = ntarrlen(args);
	bool set = false, unset = false;
	int printshopt = -1;
	int c;
	optind = 0;
	while((c = getopt(argc, args, "suqp")) != -1) switch (c){
		case 's':
			set = true;
			break;
		case 'u':
			unset = true;
			break;
		case 'q':
			printshopt = 0;
			break;
		case 'p':
			if(printshopt < 0) printshopt = 1;
			break;
		default:
			ctxt->io.log(LL_WARN, "%s: unknown option '%c'", args[0], c);
			break;
	}
	const size_t opts = optind-1;
	const bool hasopts = opts+1 < argc;
	if(printshopt < 0 && !hasopts) printshopt = 2;
	if(set && unset){
		ctxt->io.log(LL_ERROR, "%s: cannot set and unset shell options simultaneously", args[0]);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
	}
	const bool suent = set || unset;
	const bool setto = set ? true : false;
	if(suent && hasopts) for(size_t i = opts+1; i < argc; i++) switchsetopt(*ctxt->parcopts, args[i], setto, { ctxt->io.log(LL_ERROR, "%s: %s: invalid shell option name", args[0], args[i]); return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}}); });
	if(printshopt) printopts(*ctxt->parcopts, !hasopts && suent ? setto ? 1 : -1 : 0, ctxt->io, hasopts ? args+opts+1 : null);
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = suent || optsallenabled(*ctxt->parcopts, args+opts+1) ? 0 : 1}});
}
