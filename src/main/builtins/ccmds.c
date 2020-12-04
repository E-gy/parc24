#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/argsarr_mut.h>
#include <util/thread.h>
#include <stdlib.h>
#include <util/fddio.h>
#include <cppo/parallels.h>

struct pakkedi {
	ArgsArr_Mut args;
	struct exe_opts exeopts;
};
typedef struct pakkedi* TSPA;

static void pakked_destroy(TSPA a){
	if(!a) return;
	argsarrmut_destroy(a->args);
	free(a);
}

static TSPA pakked_new(argsarr args, ParContext c){
	if(!args || !c) return null;
	new(TSPA, p);
	*p = (struct pakkedi){0};
	*p = (struct pakkedi){argsarrmut_from(cpt2ptr(args)), c->exeopts};
	if(!p->args) retclean(null, {pakked_destroy(p);});
	return p;
}

static int cmd_echo_exe(TSPA a){
	for(size_t i = 1; i < a->args->size; i++){
		if(!IsOk(fddio_writestr(a->exeopts.iostreams[IOSTREAM_STD_OUT], a->args->args[i]))) retclean(1, { pakked_destroy(a); });
		if(i < a->args->size-1) if(!IsOk(fddio_writestr(a->exeopts.iostreams[IOSTREAM_STD_OUT], " "))) retclean(1, { pakked_destroy(a); });
	}
	if(!IsOk(fddio_writestr(a->exeopts.iostreams[IOSTREAM_STD_OUT], "\n"))) retclean(1, { pakked_destroy(a); });
	retclean(0, { pakked_destroy(a); });
}
static threadfwrap_reti(cmd_echo_exe);

TraverseASTResult cmd_echo(argsarr args, ParContext context){
	TSPA a = pakked_new(args, context);
	if(!a) return Error_T(travast_result, {"failed to pack up parallels args"});
	ThreadResult tr = parallels_runf(cmd_echo_exe_wrap, a, context->exeopts.background);
	return IsOk_T(tr) ? Ok_T(travast_result, {TRAV_WAIT_THREAD, {.thread = tr.r.ok}}) : Error_T(travast_result, tr.r.error);
}
