#pragma once

/**
 * @file build.h
 * Everything you need to build a parser.
 * 
 * Turns out, you don't need much... besides grammar.
 * 
 */

#include <calp/parser.h>
#include <calp/grammar.h>
#include <calp/ptypes.h>

Result_T(parser_build_result, Parser, string_v);
#define ParserBuildResult struct parser_build_result

/**
 * @param grammar @consumes
 * @returns @produces result
 */
ParserBuildResult parser_build(Grammar gr);
