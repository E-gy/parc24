#include "lexe.h"

#include <calp/ast.h>
#include <parc24/pars.h>

/**
 * Executes AST
 * 
 * @param ast @ref ast
 * @param opts @ref options
 * @param io
 * @return TihsExeResult 
 */
TihsExeResult tihs_exeast(AST ast, TihsOptions opts, ParC24IO io){
	ast_log(ast);
	return Ok_T(tihs_exe_result, 0);
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
