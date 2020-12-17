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
#include <util/buffer_printf.h>

#ifdef _WIN32
#define O_CLOEXEC 0
#endif

#define varvref(v) ((struct getvarv){false, {.ref = v}})
#define varvcopy(v) ((struct getvarv){true, {.copy = v}})

struct getvarv parcontext_getunivar(ParContext c, string v){
	if(streq("?", v)){
		Buffer buff = buffer_new(16);
		buffer_printf(buff, "%i", c->lastexit);
		return varvcopy(buffer_destr(buff));
	}
	if(streq("$", v)){
		Buffer buff = buffer_new(16);
		buffer_printf(buff, "%i", getpid());
		return varvcopy(buffer_destr(buff));
	}
	if(streq("#", v)){
		int argc = 0;
		for(; c->args[argc]; argc++);
		Buffer buff = buffer_new(16);
		buffer_printf(buff, "%i", argc);
		return varvcopy(buffer_destr(buff));
	}
	if(streq("@", v) || streq("*", v)){ //FIXME?
		Buffer buff = buffer_new(256);
		for(argsarr a = c->args; *a; a++){
			if(a != c->args) buffer_append_str(buff, " ");
			buffer_append_str(buff, *a);
		}
		return varvcopy(buffer_destr(buff));
	}
	if(streq("RANDOM", v)){
		Buffer buff = buffer_new(16);
		buffer_printf(buff, "%i", rand()%32767);
		return varvcopy(buffer_destr(buff));
	}
	if(streq("PWD", v)){
		Buffer buff = buffer_new(512);
		getcwd(buff->data, buff->cap);
		buff->size = strlen(buff->data);
		return varvcopy(buffer_destr(buff));
	}
	#ifndef _WIN32
	if(streq("UID", v)){
		Buffer buff = buffer_new(16);
		buffer_printf(buff, "%i", getuid());
		return varvcopy(buffer_destr(buff));
	}
	#endif
	IfOk_T(str2i(v), num, {
		if(num == 0) return varvref(c->currexe);
		if(num > 0 && num-1 < ntarrlen_t(c->args, long)) return varvref(c->args[num-1]); 
	});
	{
		string_mut vv = varstore_get(c->vars, v);
		if(vv) return varvref(vv);
	}
	{
		string_mut envv = getenv(v);
		if(envv) return varvref(envv);
	}
	if(streq("HOME", v)) return varvref(user_get_home(null));
	return varvref(null);
}

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
		fctxt.currexe = args[0];
		fctxt.args = args+1;
		fctxt.ios = iosstack_snapdup(fctxt.ios);
		TraverseASTResult tfr = traverse_ast(fun, &fctxt);
		iosstack_destroy(fctxt.ios);
		argsarrmut_destroy(argsmuta);
		return tfr;
	}
	CCMD ccmd = ccmdstore_get(ctxt->ccmds, args[0]);
	if(ccmd){
		TraverseASTResult ccmdr = ccmd(args, ctxt);
		argsarrmut_destroy(argsmuta);
		return ccmdr;
	}
	ExeRunResult exe = exe_runa(args, ctxt->ios, ctxt->exeback);
	argsarrmut_destroy(argsmuta);
	return IsOk_T(exe) ? Ok_T(travast_result, {TRAV_WAIT_CHILD, {.child = exe.r.ok}}) : Error_T(travast_result, exe.r.error);
}

TraverseASTResult parcontext_uniredir(enum redirection redir, int stream, string target, ParContext ctxt){
	if(!target || !ctxt) return Error_T(travast_result, {"invalid args"});
	if(stream < 0) stream = redir < REDIR_IN ? IOSTREAM_STD_OUT : IOSTREAM_STD_IN;
	const int maxstreams = sizeof(ctxt->ios)/sizeof(fd_t);
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
			if(streq(target, "-")) return !IsOk(iostack_io_close(ctxt->ios, stream)) ? Error_T(travast_result, {"failed to close target stream"}) : Ok_T(travast_result, {0});
			Str2IResult sn = str2i(target);
			if(!IsOk_T(sn)) return Error_T(travast_result, {"target is not a number"});
			if(!IsOk(iosstack_io_dup(ctxt->ios, stream, sn.r.ok))) return Error_T(travast_result, {"failed to dup stream"});
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
	if(!IsOk(iostack_io_open(ctxt->ios, stream, f))) return Error_T(travast_result, {"failed to open redirection stream"});
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
