#include "exe.h"

#include <util/null.h>
#include <calp/ast.h>
#include <parc24/pars.h>
#include <parc24/travast.h>
#include <cppo.h>
#include <stdio.h>

/**
 * Executes AST
 * 
 * @param ast @ref ast
 * @param opts @ref options
 * @param io
 * @return TihsExeResult 
 */
TihsExeResult tihs_exeast(AST ast, TihsOptions opts, ParC24IO io){
	ast_log(ast); //TODO merge(?) tihs options and ParContext
	struct parcontext ctxt = {null, {{STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}, false}, io};
	TraverseASTResult trr = traverse_ast(ast, &ctxt);
	if(!IsOk_T(trr)) return Error_T(tihs_exe_result, trr.r.error);
	if(!trr.r.ok.running) return Ok_T(tihs_exe_result, trr.r.ok.completed);
	ExeWaitResult wr = exe_waitretcode(trr.r.ok.running);
	if(!IsOk_T(wr)) return Error_T(tihs_exe_result, wr.r.error);
	return Ok_T(tihs_exe_result, wr.r.ok);
}

TihsExeResult tihs_exestr(string str, Parser parcer, TihsOptions opts, ParC24IO io){
	IfElse_T(parcer_parse(parcer, str), ast, {
		TihsExeResult exer = tihs_exeast(ast, opts, io);
		ast_destroy(ast);
		return exer;
	}, err, {
		return Error_T(tihs_exe_result, err);
	});
}
