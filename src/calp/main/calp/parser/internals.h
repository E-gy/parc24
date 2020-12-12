#pragma once

#include <calp/ptypes.h>
#include <calp/grammar/internals.h>
#include <calp/util/null.h>


// Forward Declarations

struct entinf;
typedef struct entinf* EntityInfo;

struct groupfl;
typedef struct groupfl* FirstList;

struct groupfle;
typedef struct groupfle* FirstListElement;


// First List

struct groupfl {
	FirstListElement first;
	/** @ref */ Rule fallback;
	struct {
		/** @ref */ Rule r;
		/** @ref */ Symbol from;
	} lr;
};

struct groupfle {
	/** @ref */ EntityInfo symbol;
	/** @ref */ Rule r;
	int priority;
	FirstListElement next;
};

/**
 * @returns @produces list
 */
FirstList FirstList_new();

/**
 * @param l @consumes 
 */
void FirstList_destroy(FirstList l);

/**
 * @param l @refmut
 * @param symbol @ref
 * @param r @ref
 */
Result FirstList_add(FirstList l, EntityInfo symbol, Rule r, int priority);


// Entity Info

struct entinf {
	enum {
		SYMB_TERM,
		SYMB_GROUP,
		RULE,
	} type;
	bool init;
	bool nullable;
	union {
		struct {
			/** @ref */ TerminalSymbolId symbolId;
			/** @ref */ Symbol symbol;
		} term;
		struct {
			/** @ref */ GroupId groupId;
			/** @ref */ Group group;
			FirstList firsts;
		} group;
		struct {
			/** @ref */ Rule rule;
		} rule;
	} i;
	EntityInfo mapnext;
};

#define entinf_blank_term(s) ((struct entinf){SYMB_TERM, false, false, {.term = {s, null}}, null})
#define entinf_blank_group(g) ((struct entinf){SYMB_GROUP, false, false, {.group = {g, null, null}}, null})
#define entinf_blank_rule(r) ((struct entinf){RULE, false, false, {.rule = {r}}, null})

#define entinf_blank_symbol(_s) (_s ? _s->type == SYMBOL_TYPE_TERM ? entinf_blank_term(_s->val.term.id) : _s->type == SYMBOL_TYPE_GROUP ? entinf_blank_group(_s->val.group.id) : entinf_blank_rule(null) : entinf_blank_rule(null))


// Entity Info Map

#define ENTIMAPS 256

struct entimap {
	EntityInfo ents[ENTIMAPS];
};
typedef struct entimap* EntitiesMap;

/**
 * @param i @ref
 * @param ii @copy
 */
bool entinf_eq(EntityInfo i, struct entinf ii);

/**
 * @returns @produces map
 */
EntitiesMap entimap_new();

/**
 * @param map @consumes
 */
void entimap_destroy(EntitiesMap map);

/**
 * @param map @ref
 * @param key @copy
 * @returns @ref info
 */
EntityInfo entimap_get(EntitiesMap map, struct entinf key);
/**
 * @param map @refmut
 * @param kv @copy
 * @returns @ref info
 */
EntityInfo entimap_add(EntitiesMap map, struct entinf kv);

/**
 * @param grammar @ref
 * @param map @ref
 */
void entimap_log(Grammar grammar, EntitiesMap map);


// Parser

struct parser {
	Grammar grammar;
	EntitiesMap ents;
};
