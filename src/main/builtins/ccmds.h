#pragma once

#include <ptypes.h>
#include <parc24/travast.h>

TraverseASTResult cmd_exit(argsarr args, ParContext context);
TraverseASTResult cmd_break(argsarr args, ParContext context);
TraverseASTResult cmd_continue(argsarr args, ParContext context);

TraverseASTResult cmd_alias(argsarr args, ParContext context);
TraverseASTResult cmd_unalias(argsarr args, ParContext context);

TraverseASTResult cmd_echo(argsarr args, ParContext context);
TraverseASTResult cmd_shopt(argsarr args, ParContext context);
