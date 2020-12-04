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
#define travt_is_hascode(t) (t == TRAV_COMPLETED || t == TRAV_SHRTCT_EXIT)
#define travt_is_wait(t) (TRAV_COMPLETED < t && t < TRAV_SHRTCT_EXIT)
#define travt_is_shrtct(t) (t >= TRAV_SHRTCT_EXIT)
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

enum redirection {
	REDIR_NO = 0,
	REDIR_OUT,
	REDIR_OUT_APPEND,
	REDIR_OUT_DUP,
	REDIR_OUT_CLOBBER,
	REDIR_IN,
	REDIR_IN_HERE,
	REDIR_IN_DUP,
	REDIR_INOUT,
};

TraverseASTResult parcontext_uniredir(enum redirection redir, int stream, string target, ParContext context);

TraverseASTResult parcontext_unixec(argsarr args, ParContext context);

/**
 * @param @consumes r 
 * @return @produces result 
 */
TraverseASTResult parcontext_uniwait(TraverseASTResult r);

TraverseASTResult traverse_ast(AST ast, ParContext context);
