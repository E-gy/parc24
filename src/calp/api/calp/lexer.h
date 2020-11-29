#pragma once

/**
 * @file lexer.h
 * Lexer definition.
 * 
 * Definition of what a lexer is.
 * No implementations here.
 * 
 */

#include "ptypes.h"

/**
 * Given a string finds the end of the token, returning null if the string does not begin with the token
 * @param str @ref
 * @returns @produces @ref end [exclusive]
 */
typedef string (*SelfLexingToken)(string);

Result_T(lexer_result, struct {
	/** @ref */ string start;
	/** @ref */ string end;
	/** @ref */ string next;
}, string_v);
#define LexerResult struct lexer_result

/**
 * @param str @ref
 * @param token @ref
 * @returns @produces result
 */
typedef struct lexer_result (*Lexer)(string,SelfLexingToken);
