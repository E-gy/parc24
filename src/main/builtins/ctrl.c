#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <util/str2i.h>
#include <util/argsarr_mut.h>
#include <stdlib.h>
#include <parc24/ioslog.h>

TraverseASTResult cmd_exit(argsarr args, ParContext context){
	int code = context->lastexit;
	if(args[1]){
		Str2IResult ps = str2i(args[1]);
		if(!IsOk_T(ps)){
			parciolog(context->ios, LL_ERROR, "%s: numeric argument required", args[0]);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
		}
		code = ps.r.ok;
	}
	return Ok_T(travast_result, {TRAV_SHRTCT_EXIT, {.completed = code}});
}

TraverseASTResult cmd_break(argsarr args, ParContext context){
	if(context->lupdepth == 0){
		parciolog(context->ios, LL_ERROR, "%s: makes no sense outside looops", args[0]);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 128}});
	}
	int lvl = 1;
	if(args[1]){
		Str2IResult ps = str2i(args[1]);
		if(!IsOk_T(ps)){
			parciolog(context->ios, LL_ERROR, "%s: numeric argument required", args[0]);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 128}});
		}
		lvl = ps.r.ok;
		if(lvl < 0){
			parciolog(context->ios, LL_ERROR, "%s: positive argument required", args[0]);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
		}
	}
	return Ok_T(travast_result, {TRAV_SHRTCT_BREAK, {.shortcut_depth = lvl > context->lupdepth ? context->lupdepth : lvl}});
}

TraverseASTResult cmd_continue(argsarr args, ParContext context){
	if(context->lupdepth == 0){
		parciolog(context->ios, LL_ERROR, "%s: makes no sense outside looops", args[0]);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 128}});
	}
	int lvl = 1;
	if(args[1]){
		Str2IResult ps = str2i(args[1]);
		if(!IsOk_T(ps)){
			parciolog(context->ios, LL_ERROR, "%s: numeric argument required", args[0]);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 128}});
		}
		lvl = ps.r.ok;
		if(lvl < 0){
			parciolog(context->ios, LL_ERROR, "%s: positive argument required", args[0]);
			return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
		}
	}
	return Ok_T(travast_result, {TRAV_SHRTCT_CONTINUE, {.shortcut_depth = lvl > context->lupdepth ? context->lupdepth : lvl}});
}
