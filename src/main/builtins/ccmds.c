#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/argsarr_mut.h>
#include <util/thread.h>
#include <stdlib.h>
#include <cppo/parallels.h>
#include <parc24/ioslog.h>
#include <util/buffer.h>

struct pakkedi {
	ArgsArr_Mut args;
	IOsStack ios;
	struct parc_options opts;
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
	*p = (struct pakkedi){argsarrmut_from(cpt2ptr(args)), iosstack_snapdup(c->ios), *c->parcopts};
	if(!p->args) retclean(null, {pakked_destroy(p);});
	return p;
}

#define escnumbasedigi(digi, base) { \
	char mrk = 0; \
	size_t j; \
	for(j = i+2; j < i+2+digi && j < buff->size; j++); \
	if(j == i+2+digi){ \
		mrk = buff->data[j]; \
		buff->data[j] = '\0'; \
	} \
	string_mut ent = null; \
	long hex = strtol(buff->data+i+2, &ent, base); \
	if(mrk) buff->data[j] = mrk; \
	if(!ent || ent == buff->data+i+2){ \
		repld = 0; break; \
	} \
	repl[0] = hex; \
	repld = ent-(buff->data+i); \
	break; \
}

static int cmd_echo_exe(TSPA a){
	bool omitnl = false;
	ATTR_UNUSED bool bakslsh = a->opts.xpg_echo;
	size_t i = 1;
	for(; i < a->args->size; i++) if(streq("-n", a->args->args[i])) omitnl = true; else if(streq("-e", a->args->args[i])) bakslsh = true; else if(streq("-E", a->args->args[i])) bakslsh = false; else break;
	bool juststop = false;
	for(; i < a->args->size && !juststop; i++) if(bakslsh){
		Buffer buff = buffer_new_from(a->args->args[i], -1);
		if(!buff) retclean(2, { pakked_destroy(a); });
		for(size_t i = 0; i < buff->size; i++) if(buff->data[i] == '\\'){
			char repl[] = { 0, 0 };
			size_t repld = 2;
			switch(buff->data[i+1]){
				case 'a': repl[0] = '\a'; break;
				case 'b': repl[0] = '\b'; break;
				case 'c':
					buffer_delete(buff, i, buff->size);
					repld = 0;
					omitnl = juststop = true;
					break;
				case 'e': repl[0] = 27; break;
				case 'f': repl[0] = '\f'; break;
				case 'n': repl[0] = '\n'; break;
				case 'r': repl[0] = '\r'; break;
				case 't': repl[0] = '\t'; break;
				case 'v': repl[0] = '\v'; break;
				case '\\': repl[0] = '\\'; break;
				case 'x': escnumbasedigi(2, 16);
				case '0': escnumbasedigi(3, 8);
				default: repld = 0;
			}
			if(repld > 0) buffer_splice_str(buff, i, i+repld, repl);
		}
		parcioprintf(a->ios, LL_INFO, "%s%s", buff->data, i < a->args->size-1 && !juststop ? " " : "");
		buffer_destroy(buff);
	} else parcioprintf(a->ios, LL_INFO, "%s%s", a->args->args[i], i < a->args->size-1 ? " " : "");
	if(!omitnl) parcioprintf(a->ios, LL_INFO, "\n");
	retclean(0, { pakked_destroy(a); });
}
static threadfwrap_reti(cmd_echo_exe)

TraverseASTResult cmd_echo(argsarr args, ParContext context){
	TSPA a = pakked_new(args, context);
	if(!a) return Error_T(travast_result, {"failed to pack up parallels args"});
	ThreadResult tr = parallels_runf(cmd_echo_exe_wrap, a, context->exeback);
	return IsOk_T(tr) ? Ok_T(travast_result, {TRAV_WAIT_THREAD, {.thread = tr.r.ok}}) : Error_T(travast_result, tr.r.error);
}
