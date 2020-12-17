#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/buffer_printf.h>
#include <parc24/ioslog.h>
#include <stdlib.h>
#include <cppo.h>
#include <unistd.h>
#include <limits.h>

#ifdef _WIN32
#define isabs(path) path[0] && path[1] && path[1] == ':'
#else
#define isabs(path) path[0] == '/'
#endif

TraverseASTResult cmd_cd(argsarr args, ParContext context){
	struct getvarv home = parcontext_getunivar(context, "HOME");
	string dst = args[1] ? args[1] : home.v.ref;
	if(streq("~", dst)) dst = home.v.ref;
	if(streq("-", dst)) if(!(dst = wdstack_get(context->wd, 1))) retclean(Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}}), { if(home.copy) free(home.v.copy); } );
	if(!*dst) retclean(Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}}), { if(home.copy) free(home.v.copy); } );
	Result ok = wdstack_changedir(context->wd, dst);
	if(home.copy) free(home.v.copy);
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = IsOk(ok) ? 0 : 1}});
}

TraverseASTResult cmd_pwd( ATTR_UNUSED argsarr args, ParContext context){
	string cwd = wdstack_get(context->wd, 0);
	if(cwd) parciolog(context->ios, LL_INFO, cwd);
	else return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
}
