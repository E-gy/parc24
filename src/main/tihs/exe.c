#include "exe.h"

#include <util/null.h>
#include <calp/ast.h>
#include <parc24/pars.h>
#include <parc24/travast.h>
#include <cppo.h>

/**
 * Executes AST
 * 
 * @param ast @ref ast
 * @param opts @ref options
 * @param io
 * @return TihsExeResult 
 */
TihsExeResult tihs_exeast(AST ast, ParContext ctxt){
	ast_log(ast); //TODO merge(?) tihs options and ParContext
	TraverseASTResult trr = traverse_ast(ast, ctxt);
	if(!IsOk_T(trr)) return Error_T(tihs_exe_result, trr.r.error);
	/*if(!trr.r.ok.type == TRAV_COMPLETED)*/ return Ok_T(tihs_exe_result, trr.r.ok.v.completed);
	// ExeWaitResult wr = exe_waitretcode(trr.r.ok.running);
	// if(!IsOk_T(wr)) return Error_T(tihs_exe_result, wr.r.error);
	// return Ok_T(tihs_exe_result, wr.r.ok);
}

TihsExeResult tihs_exestr(string str, ParContext ctxt){
	TihsExeResult exer = Ok_T(tihs_exe_result, 0);
	while(*str){
		IfElse_T(parcer_parse(ctxt->parcer, str), ast, {
			exer = tihs_exeast(ast.ast, ctxt);
			ast_destroy(ast.ast);
			str = ast.end;
		}, err, {
			return Error_T(tihs_exe_result, err);
		});
	}
	return exer;
}
