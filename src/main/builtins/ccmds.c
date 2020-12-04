#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/argsarr_mut.h>
#include <util/thread.h>
#include <stdlib.h>
#include <util/fddio.h>

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
	for(size_t i = 0; i < a->args->size; i++) if(!IsOk(fddio_writestr(a->exeopts.iostreams[IOSTREAM_STD_OUT], a->args->args[i]))) return 1;
	return 0;
}

static threadfwrap_reti(cmd_echo_exe);

TraverseASTResult cmd_echo(int argc, argsarr args, ParContext context){
	return Ok_T(travast_result, { TRAV_COMPLETED, {.completed = 0} });
}
