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
	if(streq("-", dst)) dst = varstore_get(context->vars, "OLDPWD");
	if(!*dst) retclean(Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}}), { if(home.copy) free(home.v.copy); } );
	char opwd[PATH_MAX];
	if(!getcwd(opwd, PATH_MAX)) retclean(Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}}), { if(home.copy) free(home.v.copy); });
	Buffer buff = buffer_new(PATH_MAX);
	if(isabs(dst)) buffer_append_str(buff, dst);
	else buffer_printf(buff, "%s/%s", opwd, dst);
	if(home.copy) free(home.v.copy);
	int chok = chdir(buff->data);
	buffer_destroy(buff);
	if(chok < 0) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
	varstore_add(context->vars, "OLDPWD", opwd); //FIXME
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
}

TraverseASTResult cmd_pwd( ATTR_UNUSED argsarr args, ParContext context){
	char cwd[PATH_MAX];
	if(getcwd(cwd, PATH_MAX)) parciolog(context->ios, LL_INFO, cwd);
	else return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
}
