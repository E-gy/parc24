#include <stdlib.h>
#include <ptypes.h>
#include <unistd.h>
#include <parc24/io.h>
#include <tihs/opts.h>
#include <parc24/pars.h>
#include <parc24/var_store.h>
#include <parc24/context.h>
#include <tihs/exe.h>
#include <util/bufferio.h>

int main(int argc, argsarr args){
	const ParC24IO io = parc24io_fromstd();
	TihsOptsParseResult optpars = tihsopts_parse_caste(args+1, io);
	IfError_T(optpars, err, {
		io.log(LL_CRITICAL, "Failed to parse arguments - %s", err.s);
		return 1;
	});
	struct tihsopts opts = optpars.r.ok;
	const Parser parcer = parcer_defolt_new();
	if(!parcer){
		io.log(LL_CRITICAL, "Failed to initalize parser. Report logs to CALP, thx.");
		return 1;
	}
	VarStore vars = varstore_new();
	if(!vars){
		io.log(LL_CRITICAL, "Failed to create variables store");
		return 1;
	}
	struct parcontext ctxt = {vars, {{STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}, false}, io, parcer};
	if(opts.commandstr || opts.commandfile){
		string_mut str = opts.commandstr;
		if(!(str = opts.commandstr)){
			BufferResult buff = buffer_from_file(opts.commandfile);
			IfError_T(buff, err, {
				io.log(LL_ERROR, "Failed to read file - %s", err.s);
				return 1;
			});
			str = buffer_destr(buff.r.ok);
		}
		TihsExeResult exer = tihs_exestr(str, &ctxt);
		IfError_T(exer, err, {
			io.log(LL_ERROR, "Execution error - %s", err.s);
			return 1;
		});
		return exer.r.ok;
	} else while(true){
		int lec = 0;
		IfElse_T(io.readline(), line, {
			if(!line) return lec;
			TihsExeResult exer = tihs_exestr(line, &ctxt);
			IfError_T(exer, err, { io.log(LL_ERROR, "Execution error - %s", err.s); });
			IfOk_T(exer, ec, { lec = ec; });
		}, err, {
			io.log(LL_ERROR, "Failed to read from standard input - %s", err.s);
		});
	}
}
