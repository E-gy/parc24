#pragma once

/**
 * @file build.h
 * Grammar construction functionality.
 * 
 * Provides functions necessary to construct a grammar at runtime.
 */

#include <calp/grammar.h>

/**
 * @param term @ref
 * @param name @ref
 * @returns @produces symbol
 */
Symbol symbol_new_term(TerminalSymbolId term, string name, int priority);
/**
 * @param group @ref
 * @returns @produces symbol
 */
Symbol symbol_new_group(GroupId group, int priority);

/**
 * @param symbol @consumes
 */
void symbol_destroy(Symbol symbol);

struct ruleb;
typedef struct ruleb* RuleBuilder;

/**
 * @returns @produces builder
 */
RuleBuilder ruleb_new(int priority);
/**
 * @param builder @refmut
 * @param symbol @consumes
 */
RuleBuilder ruleb_add(RuleBuilder builder, Symbol symbol);
/**
 * @param builder @consumes
 * @returns @produces rule
 */
Rule ruleb_uild(RuleBuilder builder);

struct groupb;
typedef struct groupb* GroupBuilder;

/**
 * @param id @ref
 * @param name @ref
 * @returns @produces builder
 */
GroupBuilder groupb_new(GroupId id, string name, int priority);
/**
 * @param builder @refmut
 * @param rule @consumes
 */
GroupBuilder groupb_add(GroupBuilder builder, Rule rule);
/**
 * @param builder @consumes
 * @returns @produces group
 */
Group groupb_uild(GroupBuilder builder);

struct gramb;
typedef struct gramb* GrammarBuilder;

/**
 * @returns @produces builder
 */
GrammarBuilder gramb_new(string name);
/**
 * @param builder @refmut
 * @param group @consumes
 */
GrammarBuilder gramb_add(GrammarBuilder builder, Group group);
/**
 * @param builder @consumes
 * @returns @produces grammar
 */
Grammar gramb_uild(GrammarBuilder builder);
