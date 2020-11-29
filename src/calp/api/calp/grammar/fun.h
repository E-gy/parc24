#pragma once

/**
 * @file fun.h
 * Grammar functions.
 * 
 * Everything you can do with the grammar itself.
 * 
 */

#include <calp/grammar.h>

/**
 * @param grammar @ref
 * @param symbols @ref
 */
void symbols_logi(Grammar grammar, Symbol symbols);

/**
 * @param grammar @ref
 */
void grammar_log(Grammar grammar);
