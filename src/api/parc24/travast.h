#pragma once

#include <calp/ast.h>
#include <cppo/types.h>
#include "context.h"

Result_T(travast_result, struct { ChildProcessInfo running; int completed; }, string_v);
#define TraverseASTResult struct travast_result

TraverseASTResult traverse_ast(AST ast, ParContext context);
