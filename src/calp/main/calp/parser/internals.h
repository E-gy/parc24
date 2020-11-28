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
	Rule fallback;
	struct {
		Rule r;
		Symbol from;
	} lr;
};

struct groupfle {
	/** @ref */ EntityInfo symbol;
	/** @ref */ Rule r;
	FirstListElement next;
};

/**
 * @produces list
 */
FirstList FirstList_new();
/**
 * @refmut l
 * @ref symbol
 * @ref r
 */
Result FirstList_add(FirstList l, EntityInfo symbol, Rule r);


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
			TerminalSymbolId symbolId;
			Symbol symbol;
		} term;
		struct {
			GroupId groupId;
			Group group;
			FirstList firsts;
		} group;
		struct {
			Rule rule;
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
 * @ref i
 * @copy ii
 */
bool entinf_eq(EntityInfo i, struct entinf ii);

/**
 * @produces map
 */
EntitiesMap entimap_new();
/**
 * @ref map
 * @copy key
 * @returns @ref info
 */
EntityInfo entimap_get(EntitiesMap map, struct entinf key);
/**
 * @refmut map
 * @copy kv
 * @returns @ref info
 */
EntityInfo entimap_add(EntitiesMap map, struct entinf kv);

/**
 * @ref grammar
 * @ref map
 */
void entimap_log(Grammar grammar, EntitiesMap map);


// Parser

struct parser {
	Grammar grammar;
	EntitiesMap ents;
};
