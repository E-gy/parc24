#include <stdlib.h>
#include <ptypes.h>
#include <unistd.h>
#include <stdio.h>
#include <parc24/ioslog.h>
#include <tihs/opts.h>
#include <parc24/pars.h>
#include <parc24/var_store.h>
#include <parc24/context.h>
#include <tihs/exe.h>
#include <util/bufferio.h>
#include <builtins/ccmds.h>

ParC24IOReadResult parcio_derp_std_read(void);

int main(int argc, argsarr args){
	IOsStack ios = parcio_new_fromstd();
	if(!ios){
		fprintf(stderr, "[SUPERCRITICAL] Failed to create io stack\n");
		return 1;
	}
	TihsOptsParseResult optpars = tihsopts_parse_caste(args+1, ios);
	IfError_T(optpars, err, {
		parciolog(ios, LL_CRITICAL, "Failed to parse arguments - %s", err.s);
		return 1;
	});
	struct tihsopts opts = optpars.r.ok;
	const Parser parcer = parcer_defolt_new();
	if(!parcer){
		parciolog(ios, LL_CRITICAL, "Failed to initalize parser. Report logs to CALP, thx.");
		return 1;
	}
	VarStore vars = varstore_new();
	if(!vars || !IsOk(varstore_add(vars, "IFS", " \t\n"))){
		parciolog(ios, LL_CRITICAL, "Failed to initialize variables store");
		return 1;
	}
	FuncStore funcs = funcstore_new();
	if(!funcs){
		parciolog(ios, LL_CRITICAL, "Failed to create functions store");
		return 1;
	}
	CCMDStore ccmds = ccmdstore_new();
	if(!ccmds){
		parciolog(ios, LL_CRITICAL, "Failed to create ccmds store");
		return 1;
	}
	if(
		!IsOk(ccmdstore_set(ccmds, "exit", cmd_exit)) ||
		!IsOk(ccmdstore_set(ccmds, "break", cmd_break)) ||
		!IsOk(ccmdstore_set(ccmds, "continue", cmd_continue)) ||
		!IsOk(ccmdstore_set(ccmds, "echo", cmd_echo)) ||
		!IsOk(ccmdstore_set(ccmds, "shopt", cmd_shopt))
	){
		parciolog(ios, LL_CRITICAL, "Failed to register builtin ccmds");
		return 1;
	}
	AliasStore aliases = aliastore_new();
	if(!ccmds){
		parciolog(ios, LL_CRITICAL, "Failed to create aliases store");
		return 1;
	}
	struct parcontext ctxt = {vars, funcs, ccmds, aliases, ios, args[0], opts.args, 0, false, &opts.parcopts, parcer};
	if(opts.commandstr || opts.commandfile){
		string_mut str = opts.commandstr;
		if(!(str = opts.commandstr)){
			BufferResult buff = buffer_from_file(opts.commandfile);
			IfError_T(buff, err, {
				parciolog(ios, LL_ERROR, "Failed to read file - %s", err.s);
				return 1;
			});
			str = buffer_destr(buff.r.ok);
		}
		TihsExeResult exer = tihs_exestr(str, &ctxt);
		IfError_T(exer, err, {
			parciolog(ios, LL_ERROR, "Execution error - %s", err.s);
			return 1;
		});
		return exer.r.ok.code;
	} else for(bool exit = false; !exit;){
		if(isatty(STDIN_FILENO)){ //FIXME shouldn't `io` be used for that..?
			printf("42sh> ");
			fflush(stdout);
		}
		IfElse_T(parcio_derp_std_read(), line, {
			if(!line) return ctxt.lastexit;
			TihsExeResult exer = tihs_exestr(line, &ctxt);
			IfError_T(exer, err, { parciolog(ios, LL_ERROR, "Execution error - %s", err.s); });
			IfOk_T(exer, ec, { ctxt.lastexit = ec.code; exit = ec.exit; });
		}, err, {
			parciolog(ios, LL_ERROR, "Failed to read from standard input - %s", err.s);
		});
	}
	aliastore_destroy(aliases);
	ccmdstore_destroy(ccmds);
	funcstore_destroy(funcs);
	varstore_destroy(vars);
	parser_destroy(parcer);
	iosstack_destroy(ios);
}
