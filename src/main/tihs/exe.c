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
	// ast_log(ast); //TODO merge(?) tihs options and ParContext
	TraverseASTResult trr = traverse_ast(ast, ctxt);
	if(!ctxt->exeback) trr = parcontext_uniwait(trr);
	if(!IsOk_T(trr)) return Error_T(tihs_exe_result, trr.r.error);
	return Ok_T(tihs_exe_result, {!ctxt->exeback && travt_is_hascode(trr.r.ok.type) ? trr.r.ok.v.completed : 0, trr.r.ok.type == TRAV_SHRTCT_EXIT});
}

TihsExeResult tihs_exestr(string str, ParContext ctxt){
	TihsExeResult exer = Ok_T(tihs_exe_result, {ctxt->lastexit, false});
	while(*str && !exer.r.ok.exit){
		ctxt->lastexit = exer.r.ok.code;
		IfElse_T(parcer_parse(ctxt->parcer, str, false), ast, {
			exer = tihs_exeast(ast.ast, ctxt);
			ast_destroy(ast.ast);
			str = ast.end;
		}, err, {
			return Error_T(tihs_exe_result, err.err);
		});
	}
	return exer;
}

TihsExeCGMResult tihs_exestr_cgm(string str, ParContext ctxt){
	TihsExeCGMResult exer = Ok_T(tihs_exe_result_cgm, {ctxt->lastexit, false});
	while(*str && !exer.r.ok.exit){
		ctxt->lastexit = exer.r.ok.code;
		IfElse_T(parcer_parse(ctxt->parcer, str, true), ast, {
			TihsExeResult exer_ = tihs_exeast(ast.ast, ctxt);
			exer = IsOk_T(exer_) ? Ok_T(tihs_exe_result_cgm, {exer_.r.ok.code, exer_.r.ok.exit}) : Error_T(tihs_exe_result_cgm, {false, exer_.r.error});
			ast_destroy(ast.ast);
			str = ast.end;
		}, err, {
			return Error_T(tihs_exe_result_cgm, {err.needmore, err.err});
		});
	}
	return exer;
}
