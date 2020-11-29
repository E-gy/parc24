#pragma once

/**
 * @file lexers.h
 * Common lexers.
 * 
 * Some commonly used lexer implementations.
 * 
 */

#include "lexer.h"

LexerResult lexer0(string string, SelfLexingToken token);

LexerResult lexer_spacebegone(string string, SelfLexingToken token);
