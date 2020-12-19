#pragma once

#include <parc24/context.h>
#include <tihs/opts.h>
#include <calp/parser.h>

Result_T(tihs_exe_result, struct {
	int code;
	bool exit;
}, string_v);
#define TihsExeResult struct tihs_exe_result

Result_T(tihs_exe_result_cgm, struct {
	int code;
	bool exit;
}, struct { bool needmore; string_v err; });
#define TihsExeCGMResult struct tihs_exe_result_cgm

/**
 * Parses & executes string
 * 
 * @param str @ref input
 * @param ctxt @ref
 * @return TihsExeResult 
 */
TihsExeResult tihs_exestr(string str, ParContext ctxt);

/**
 * Parses & executes string
 * 
 * @param str @ref input
 * @param ctxt @ref
 * @return TihsExeResult 
 */
TihsExeCGMResult tihs_exestr_cgm(string str, ParContext ctxte);
