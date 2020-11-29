#pragma once

#include <calp/ast.h>
#include <calp/parser.h>

Parser parcer_defolt_new(void);

Result_T(parce_result, AST, string_v);
#define ParceResult struct parce_result

ParceResult parcer_parse(Parser parcer, string input);
