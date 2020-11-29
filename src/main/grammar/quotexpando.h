#pragma once

/**
 * @file quotexpando.h
 * Quotes & Expansion groups capture (lexing) & expansion (execution).
 * 
 */

#include <ptypes.h>

#define capture_isquotstart(str) (str && (str[0] == '\'' || str[0] == '"'))
#define capture_isexpandostart(str) (str && (str[0] == '`' || strpref("$(", str)))

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
 * Attempts to capture a word 
 * 
 * @param str @ref
 * @return @ref right after the end of word, or null if capture error 
 */
string capture_word(string str);
