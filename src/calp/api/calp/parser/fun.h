#pragma once

#include <calp/ptypes.h>
#include <calp/parser.h>
#include <calp/lexer.h>
#include <calp/ast.h>

Result_T(parse_result, AST, string_v);
#define ParseResult struct parse_result

/**
 * @ref parser
 * @ref lexer
 * @ref input
 * @ref g0
 * @produces result
 */
ParseResult parser_parse(Parser parser, Lexer lexer, string input, GroupId g0);
