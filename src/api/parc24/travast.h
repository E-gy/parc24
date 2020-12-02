#pragma once

#include <calp/ast.h>
#include <cppo/types.h>
#include "context.h"

enum travast_shrortcut_type {
	SHRTCT_NO = 0,
	SHRTCT_EXIT,
	SHRTCT_BREAK,
	SHRTCT_CONTINUE
};
Result_T(travast_result, struct {
	ChildProcessInfo running;
	int completed;
	enum travast_shrortcut_type shortcut;
	int shortcut_depth;
}, string_v);
#define TraverseASTResult struct travast_result

TraverseASTResult traverse_ast(AST ast, ParContext context);
