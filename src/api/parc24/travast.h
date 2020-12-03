#pragma once

#include <calp/ast.h>
#include <cppo/types.h>
#include "context.h"

enum travast_result_type {
	TRAV_COMPLETED = 0,
	TRAV_WAIT_CHILD,
	TRAV_WAIT_THREAD,
	TRAV_SHRTCT_EXIT,
	TRAV_SHRTCT_BREAK,
	TRAV_SHRTCT_CONTINUE
};
Result_T(travast_result, struct {
	enum travast_result_type type;
	union {
		int completed;
		ChildProcessInfo child;
		thread_t thread;
		int shortcut_depth;
	} v;
}, string_v);
#define TraverseASTResult struct travast_result

TraverseASTResult parcontext_unixec(argsarr args, ParContext context);

/**
 * @param @consumes r 
 * @return @produces result 
 */
TraverseASTResult parcontext_uniwait(TraverseASTResult r);

TraverseASTResult traverse_ast(AST ast, ParContext context);
