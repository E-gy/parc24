#include <parc24/travast.h>

#include <cppo.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <util/string.h>
#include <util/null.h>
#include <util/caste.h>
#include <util/str2i.h>
#include <cppo/parallels.h>
#include <grammar/aliaser.h>

#ifdef _WIN32
#define O_CLOEXEC 0
#endif

TraverseASTResult parcontext_unixec(argsarr args, ParContext ctxt){
	if(!args || !ctxt) return Error_T(travast_result, {"invalid args"});
	if(!args[0]) return Error_T(travast_result, {"no args given"});
	RealiasResult aliased = realias(cpt2ptr(args), ctxt->aliases);
	if(!IsOk_T(aliased)) return Error_T(travast_result, aliased.r.error);
	ArgsArr_Mut argsmuta = aliased.r.ok;
	if(argsmuta) args = argsmuta->args;
	Funcmd fun = funcstore_get(ctxt->funcs, args[0]);
	if(fun){
		struct parcontext fctxt = *ctxt;
		fctxt.args = args;
		TraverseASTResult tfr = traverse_ast(fun, &fctxt);
		argsarrmut_destroy(argsmuta);
		return tfr;
	}
	CCMD ccmd = ccmdstore_get(ctxt->ccmds, args[0]);
	if(ccmd){
		TraverseASTResult ccmdr = ccmd(args, ctxt);
		argsarrmut_destroy(argsmuta);
		return ccmdr;
	}
	ExeRunResult exe = exe_runa(args, ctxt->exeopts);
	argsarrmut_destroy(argsmuta);
	return IsOk_T(exe) ? Ok_T(travast_result, {TRAV_WAIT_CHILD, {.child = exe.r.ok}}) : Error_T(travast_result, exe.r.error);
}

TraverseASTResult parcontext_uniredir(enum redirection redir, int stream, string target, ParContext ctxt){
	if(!target || !ctxt) return Error_T(travast_result, {"invalid args"});
	if(stream < 0) stream = redir < REDIR_IN ? IOSTREAM_STD_OUT : IOSTREAM_STD_IN;
	const int maxstreams = sizeof(ctxt->exeopts.iostreams)/sizeof(fd_t);
	if(stream >= maxstreams) return Error_T(travast_result, {"stream # outside of supported bounds"});
	const bool noclobber = false;//FIXME use tihs options
	fd_t f = -1;
	switch(redir){
		case REDIR_NO: return Ok_T(travast_result, {0});
		case REDIR_OUT:
		case REDIR_OUT_CLOBBER: {
			if(noclobber && redir != REDIR_OUT_CLOBBER){
				struct stat fi;
				if(stat(target, &fi) < 0){
					if(errno != ENOENT) return Error_T(travast_result, {"failed to test target"}); 
				} else if(S_ISREG(fi.st_mode)) return Error_T(travast_result, {"target already exists"}); 
			}
			f = open(target, O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			break;
		}
		case REDIR_OUT_APPEND:
			f = open(target, O_WRONLY|O_CREAT|O_APPEND|O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			break;
		case REDIR_IN:
			f = open(target, O_RDONLY|O_CLOEXEC);
			break;
		case REDIR_IN_HERE: {
			PipeResult pipe = pipe_new();
			if(!IsOk_T(pipe)) return Error_T(travast_result, pipe.r.error);
			ThreadResult pr = parallels_writestr(pipe.r.ok.write, target, true);
			if(!IsOk_T(pr)) retclean(Error_T(travast_result, pr.r.error), { close(pipe.r.ok.write); close(pipe.r.ok.read); });
			f = pipe.r.ok.read;
			break;
		}
		case REDIR_OUT_DUP:
		case REDIR_IN_DUP: {
			if(streq(target, "-")) return close(ctxt->exeopts.iostreams[stream]) < 0 ? Error_T(travast_result, {"failed to close target stream"}) : Ok_T(travast_result, {0});
			Str2IResult sn = str2i(target);
			if(!IsOk_T(sn)) return Error_T(travast_result, {"target is not a number"});
			f = ctxt->exeopts.iostreams[sn.r.ok];
			//TODO check readable/writeable
			break;
		}
		case REDIR_INOUT:
			f = open(target, O_RDWR|O_CLOEXEC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			break;
		default:
			return Error_T(travast_result, {"unknown redirection"});
	}
	if(f < 0) return Error_T(travast_result, {"failed to open redirection target"});
	ctxt->exeopts.iostreams[stream] = f;
	return Ok_T(travast_result, {0});
}

TraverseASTResult parcontext_uniwait(TraverseASTResult r){
	if(!IsOk_T(r)) return r;
	switch(r.r.ok.type){
		case TRAV_WAIT_CHILD: {
			ExeWaitResult cw = exe_waitretcode(r.r.ok.v.child);
			return IsOk_T(cw) ? Ok_T(travast_result, {TRAV_COMPLETED, {.completed = cw.r.ok}}) : Error_T(travast_result, cw.r.error);
		}
		case TRAV_WAIT_THREAD: {
			ExeWaitResult cw = exethread_waitretcode(r.r.ok.v.thread);
			return IsOk_T(cw) ? Ok_T(travast_result, {TRAV_COMPLETED, {.completed = cw.r.ok}}) : Error_T(travast_result, cw.r.error);
		}
		default: return r;
	}
}
