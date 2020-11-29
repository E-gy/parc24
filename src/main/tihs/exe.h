#pragma once

#include <parc24/context.h>
#include <tihs/opts.h>
#include <calp/parser.h>

Result_T(tihs_exe_result, int, string_v);
#define TihsExeResult struct tihs_exe_result

/**
 * Parses & executes string
 * 
 * @param str @ref input
 * @param ctxt @ref
 * @return TihsExeResult 
 */
TihsExeResult tihs_exestr(string str, ParContext ctxt);
