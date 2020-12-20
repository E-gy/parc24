#include "ccmds.h"

#include <util/null.h>
#include <util/string.h>
#include <parc24/ioslog.h>
#include <util/bufferio.h>
#include <tihs/exe.h>

TraverseASTResult cmd_source(argsarr args, ParContext context){
	if(!args[1]){
		parciolog(context->ios, LL_ERROR, "%s: filename argument required", args[1]);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	}
	BufferResult buff = buffer_from_file(args[1]);
	if(!IsOk_T(buff)){
		parciolog(context->ios, LL_ERROR, "%s: read error: %s", args[1], buff.r.error);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 1}});
	}
	struct parcontext cctxt = *context;
	if(args[2]) cctxt.args = args+2;
	TihsExeResult exer = tihs_exestr(buff.r.ok->data, &cctxt);
	buffer_destroy(buff.r.ok);
	if(!IsOk_T(exer)){
		parciolog(context->ios, LL_ERROR, "%s: execution error: %s", args[1], exer.r.error);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 2}});
	}
	return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = exer.r.ok.code}});
}
