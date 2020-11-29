#pragma once

#include <parc24/io.h>
#include <tihs/opts.h>
#include <calp/parser.h>

Result_T(tihs_exe_result, int, string_v);
#define TihsExeResult struct tihs_exe_result

/**
 * Parses & executes string
 * 
 * @param str @ref input
 * @param parcer @ref parser
 * @param opts @ref
 * @param io
 * @return TihsExeResult 
 */
TihsExeResult tihs_exestr(string str, Parser parcer, TihsOptions opts, ParC24IO io);
