#pragma once

#include "ptypes.h"

/**
 * Given a string finds the end of the token, returning null if the string does not begin with the token
 * @ref str
 * @produces @ref end [exclusive]
 */
typedef string (*SelfLexingToken)(string);

Result_T(lexer_result, struct {
	/** @ref */ string start;
	/** @ref */ string end;
	/** @ref */ string next;
}, string_v);
#define LexerResult struct lexer_result

/**
 * @ref str
 * @ref token
 * @produces result
 */
typedef struct lexer_result (*Lexer)(string,SelfLexingToken);
