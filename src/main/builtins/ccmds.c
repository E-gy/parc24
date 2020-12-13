#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/argsarr_mut.h>
#include <util/thread.h>
#include <stdlib.h>
#include <cppo/parallels.h>
#include <parc24/ioslog.h>

struct pakkedi {
	ArgsArr_Mut args;
	IOsStack ios;
};
typedef struct pakkedi* TSPA;

static void pakked_destroy(TSPA a){
	if(!a) return;
	argsarrmut_destroy(a->args);
	iosstack_destroy(a->ios);
	free(a);
}

static TSPA pakked_new(argsarr args, ParContext c){
	if(!args || !c) return null;
	new(TSPA, p);
	*p = (struct pakkedi){0};
	*p = (struct pakkedi){argsarrmut_from(cpt2ptr(args)), iosstack_snapdup(c->ios)};
	if(!p->args) retclean(null, {pakked_destroy(p);});
	return p;
}

static int cmd_echo_exe(TSPA a){
	bool omitnl = false;
	ATTR_UNUSED bool bakslsh = false; //TODO baskslsh processing
	size_t i = 1;
	for(; i < a->args->size; i++) if(streq("-n", a->args->args[i])) omitnl = true; else if(streq("-e", a->args->args[i])) bakslsh = true; else if(streq("-E", a->args->args[i])) bakslsh = false; else break;
	for(; i < a->args->size; i++) parcioprintf(a->ios, LL_INFO, "%s%s", a->args->args[i], i < a->args->size-1 ? " " : omitnl ? "" : "\n");
	retclean(0, { pakked_destroy(a); });
}
static threadfwrap_reti(cmd_echo_exe)

TraverseASTResult cmd_echo(argsarr args, ParContext context){
	TSPA a = pakked_new(args, context);
	if(!a) return Error_T(travast_result, {"failed to pack up parallels args"});
	ThreadResult tr = parallels_runf(cmd_echo_exe_wrap, a, context->exeback);
	return IsOk_T(tr) ? Ok_T(travast_result, {TRAV_WAIT_THREAD, {.thread = tr.r.ok}}) : Error_T(travast_result, tr.r.error);
}
