#pragma once

/**
 * @file grammar.h
 * Grammar types.
 * 
 * And only - for functionality look for dedicated imports in respective folder.
 * (destructors are considered part of type declaration)
 * 
 */

#include "ptypes.h"

struct symbol;
typedef struct symbol* Symbol;

struct rule;
typedef struct rule* Rule;

struct group;
typedef struct group* Group;

struct grammar;
typedef struct grammar* Grammar;

/**
 * Id of the terminal symbol is also the predicate and the consumer:
 * - it uniquely globally idenitifies the symbol
 * - executing it on a string tests whether given string starts with the symbol
 * - executing it on a string that starts with the symbol returns the character [right after] the end of the symbol
 */
typedef string (*TerminalSymbolId)(string);
typedef Group (*GroupId)(void);

/**
 * @param grammar @consumes 
 */
void grammar_destroy(Grammar grammar);
