#pragma once

#include <calp/parser.h>
#include <calp/grammar.h>
#include <calp/ptypes.h>

Result_T(parser_build_result, Parser, string_v);
#define ParserBuildResult struct parser_build_result

/**
 * @consumes grammar
 * @produces result
 */
ParserBuildResult parser_build(Grammar gr);
