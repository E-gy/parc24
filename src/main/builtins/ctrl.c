#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/str2i.h>
#include <util/argsarr_mut.h>
#include <stdlib.h>

TraverseASTResult cmd_exit(argsarr args, ParContext context){
	int code = -1;
	if(args[1]) IfOk_T(str2i(args[1]), c, { code = c; });
	return Ok_T(travast_result, {TRAV_SHRTCT_EXIT, {.completed = code}});
}

TraverseASTResult cmd_break(argsarr args, ParContext context){
	int lvl = 1;
	if(args[1]) IfOk_T(str2i(args[1]), l, { if(l > 0) lvl = l; });
	return Ok_T(travast_result, {TRAV_SHRTCT_BREAK, {.shortcut_depth = lvl}});
}

TraverseASTResult cmd_continue(argsarr args, ParContext context){
	int lvl = 1;
	if(args[1]) IfOk_T(str2i(args[1]), l, { if(l > 0) lvl = l; });
	return Ok_T(travast_result, {TRAV_SHRTCT_CONTINUE, {.shortcut_depth = lvl}});
}
