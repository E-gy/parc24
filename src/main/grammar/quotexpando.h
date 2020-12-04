#pragma once

/**
 * @file quotexpando.h
 * Quotes & Expansion groups capture (lexing) & expansion (execution).
 * 
 */

#include <ptypes.h>
#include <parc24/context.h>

#define capture_isquotstart(str) (str && (str[0] == '\'' || str[0] == '"'))
#define capture_isexpandostart(str) (str && (str[0] == '`' || strpref("$(", str)))
#define capture_isvariablestart(str) (str && (str[0] == '$'))

/**
 * Attempts to capture quoted subword
 * 
 * @param str @ref string beginning with `'` `"`
 * @return @ref right after the end of subword, or null if capture error
 */
string capture_quot(string str);

/**
 * Attempts to capture expansion group subword
 * 
 * @param str @ref string beginning with "`" or `$(`
 * @return @ref right after the end of subword, or null if capture error
 */
string capture_expando(string str);

/**
 * Attempts to capture variable subword
 * 
 * @param str @ref string beginning with "$""
 * @return @ref right after the end of subword, or null if capture error
 */
string capture_variable(string str);

/**
 * Attempts to capture a word 
 * 
 * @param str @ref
 * @return @ref right after the end of word, or null if capture error 
 */
string capture_word(string str);

struct expando_targets {
	bool tilde;
	bool parvar;
	bool arithmetics;
	bool command;
	bool process;
	bool path;
	bool quot;
};
#define expando_targets_all ((struct expando_targets){ .tilde = true, .parvar = true, .arithmetics = true, .command = true, .process = true, .path = true, .quot = true })
#define expando_targets_quot ((struct expando_targets){.quot=true})
#define expando_targets_parc ((struct expando_targets){.parvar=true, .arithmetics = true, .command = true})

Result_T(expando_result, string_mut, string_v);
#define ExpandoResult struct expando_result

/**
 * Expands the word
 * 
 * @param str @ref word getting expanded
 * @param what gets expanded
 * @param context @ref
 * @return @produces expanded word 
 */
ExpandoResult expando_word(string str, struct expando_targets what, ParContext context);
