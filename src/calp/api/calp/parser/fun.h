#pragma once

/**
 * @file fun.h
 * Everything you can do with a parser
 * 
 */

#include <calp/ptypes.h>
#include <calp/parser.h>
#include <calp/lexer.h>
#include <calp/ast.h>

Result_T(parse_result, AST, string_v);
#define ParseResult struct parse_result

/**
 * @param parser @ref
 * @param lexer @ref
 * @param input @ref
 * @param g0 @ref
 * @returns @produces result
 */
ParseResult parser_parse(Parser parser, Lexer lexer, string input, GroupId g0);
