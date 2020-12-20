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

static int dothething(struct tihsopts opts, ParContext ctxt){
	if(opts.commandstr || opts.commandfile){
		string_mut str = opts.commandstr;
		if(!(str = opts.commandstr)){
			BufferResult buff = buffer_from_file(opts.commandfile);
			IfError_T(buff, err, {
				parciolog(ctxt->ios, LL_ERROR, "Failed to read file - %s", err.s);
				return 2;
			});
			str = buffer_destr(buff.r.ok);
		}
		TihsExeResult exer = tihs_exestr(str, ctxt);
		if(str != opts.commandstr) free(str);
		IfError_T(exer, err, {
			parciolog(ctxt->ios, LL_ERROR, "Execution error - %s", err.s);
			return 2;
		});
		return exer.r.ok.code;
	} else for(bool exit = false; !exit;){
		if(isatty(STDIN_FILENO)){ //FIXME shouldn't `io` be used for that..?
			printf("42sh> ");
			fflush(stdout);
		}
		IfElse_T(parcio_derp_std_read(), line, {
			if(!line) return ctxt->lastexit;
			TihsExeCGMResult exer = tihs_exestr_cgm(line, ctxt);
			if(!IsOk_T(exer) && exer.r.error.needmore){
				Buffer li = buffer_new_from(line, -1);
				free(line);
				while(!IsOk_T(exer) && exer.r.error.needmore){
					if(isatty(STDIN_FILENO)){ //FIXME shouldn't `io` be used for that..?
						printf("    > ");
						fflush(stdout);
					}
					ParC24IOReadResult nl = parcio_derp_std_read();
					if(!IsOk_T(nl) || !nl.r.ok) break;
					buffer_append_str(li, "\n");
					buffer_append_str(li, nl.r.ok);
					free(nl.r.ok);
					exer = tihs_exestr_cgm(li->data, ctxt);
				}
				buffer_destroy(li);
			} else free(line);
			IfError_T(exer, err, { parciolog(ctxt->ios, LL_ERROR, "Execution error - %s", err.err.s); ctxt->lastexit = 2; });
			IfOk_T(exer, ec, { ctxt->lastexit = ec.code; exit = ec.exit; });
		}, err, {
			parciolog(ctxt->ios, LL_ERROR, "Failed to read from standard input - %s", err.s);
		});
	}
	return ctxt->lastexit;
}

int main(ATTR_UNUSED int argc, argsarr args){
	IOsStack ios = parcio_new_fromstd();
	if(!ios){
		fprintf(stderr, "[SUPERCRITICAL] Failed to create io stack\n");
		return 3;
	}
	TihsOptsParseResult optpars = tihsopts_parse_caste(args+1, ios);
	IfError_T(optpars, err, {
		parciolog(ios, LL_CRITICAL, "Failed to parse arguments - %s", err.s);
		return 3;
	});
	struct tihsopts opts = optpars.r.ok;
	const Parser parcer = parcer_defolt_new();
	if(!parcer){
		parciolog(ios, LL_CRITICAL, "Failed to initalize parser.");
		return 3;
	}
	WorkDirStack wd = wdstack_new();
	if(!wd){
		parciolog(ios, LL_CRITICAL, "Failed to create work dir stack");
		return 3;
	}
	VarStore vars = varstore_new();
	if(!vars || !IsOk(varstore_add(vars, "IFS", " \t\n"))){
		parciolog(ios, LL_CRITICAL, "Failed to initialize variables store");
		return 3;
	}
	FuncStore funcs = funcstore_new();
	if(!funcs){
		parciolog(ios, LL_CRITICAL, "Failed to create functions store");
		return 3;
	}
	CCMDStore ccmds = ccmdstore_new();
	if(!ccmds){
		parciolog(ios, LL_CRITICAL, "Failed to create ccmds store");
		return 3;
	}
	if(
		!IsOk(ccmdstore_set(ccmds, "exit", cmd_exit)) ||
		!IsOk(ccmdstore_set(ccmds, "break", cmd_break)) ||
		!IsOk(ccmdstore_set(ccmds, "continue", cmd_continue)) ||
		!IsOk(ccmdstore_set(ccmds, "alias", cmd_alias)) ||
		!IsOk(ccmdstore_set(ccmds, "unalias", cmd_unalias)) ||
		!IsOk(ccmdstore_set(ccmds, "cd", cmd_cd)) ||
		!IsOk(ccmdstore_set(ccmds, "pwd", cmd_pwd)) ||
		!IsOk(ccmdstore_set(ccmds, "source", cmd_source)) ||
		!IsOk(ccmdstore_set(ccmds, ".", cmd_source)) ||
		!IsOk(ccmdstore_set(ccmds, "echo", cmd_echo)) ||
		!IsOk(ccmdstore_set(ccmds, "shopt", cmd_shopt))
	){
		parciolog(ios, LL_CRITICAL, "Failed to register builtin ccmds");
		return 3;
	}
	AliasStore aliases = aliastore_new();
	if(!ccmds){
		parciolog(ios, LL_CRITICAL, "Failed to create aliases store");
		return 3;
	}
	PatternCompiler patco = patcomp_new();
	if(!patco){
		parciolog(ios, LL_CRITICAL, "Failed to initialize patterns");
		return 3;
	}
	Arithmetics arith = arith_new();
	if(!arith){
		parciolog(ios, LL_CRITICAL, "Failed to initialize arithmetics");
		return 3;
	}
	struct parcontext ctxt = {vars, vars, funcs, ccmds, aliases, ios, wd, patco, arith, args[0], opts.args, 0, false, &opts.parcopts, parcer};
	int ec = dothething(opts, &ctxt);
	arith_destroy(arith);
	patcomp_destroy(patco);
	aliastore_destroy(aliases);
	ccmdstore_destroy(ccmds);
	funcstore_destroy(funcs);
	varstore_destroy(vars);
	wdstack_destroy(wd);
	parser_destroy(parcer);
	iosstack_destroy(ios);
	return ec;
}
