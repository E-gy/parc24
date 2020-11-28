#include <calp/grammar/build.h>

#include "internals.h"
#include <stdlib.h>
#include <calp/util/null.h>

Symbol symbol_new_term(TerminalSymbolId term, string name){
	new(Symbol, tok);
	*tok = (struct symbol){ SYMBOL_TYPE_TERM, {.term = {term, name}}, null };
	return tok;
}

Symbol symbol_new_group(GroupId group){
	new(Symbol, tok);
	*tok = (struct symbol){ SYMBOL_TYPE_GROUP, {.group = {group}}, null };
	return tok;
}

void symbol_destroy(Symbol s){
	if(!s) return;
	free(s);
}

struct ruleb {
	Symbol first;
	Symbol last;
};

RuleBuilder ruleb_new(){
	new(RuleBuilder, b);
	*b = (struct ruleb){null, null};
	return b;
}

RuleBuilder ruleb_add(RuleBuilder b, Symbol s){
	if(!b || !s) return b;
	s->next = null;
	if(!b->first) b->first = b->last = s;
	else b->last = (b->last->next = s);
	return b;
}

Rule ruleb_uild(RuleBuilder b){
	if(!b) return null;
	new(Rule, r);
	size_t sc = 0;
	for(Symbol s = b->first; s; s = s->next) sc++;
	*r = (struct rule){b->first, sc, null};
	free(b);
	return r;
}

struct groupb {
	GroupId id;
	string name;
	Rule first;
	Rule last;
};

GroupBuilder groupb_new(GroupId id, string name){
	new(GroupBuilder, b);
	*b = (struct groupb){id, name, null, null};
	return b;
}

GroupBuilder groupb_add(GroupBuilder b, Rule r){
	if(!b || !r) return b;
	r->next = null;
	if(!b->first) b->first = b->last = r;
	else b->last = (b->last->next = r);
	return b;
}

Group groupb_uild(GroupBuilder b){
	if(!b) return null;
	new(Group, g);
	size_t rc = 0;
	for(Rule r = b->first; r; r = r->next) rc++;
	*g = (struct group){b->id, b->name, b->first, rc, null};
	free(b);
	return g;
}

struct gramb {
	string name;
	Group first;
	Group last;
};

GrammarBuilder gramb_new(string name){
	new(GrammarBuilder, b);
	*b = (struct gramb){name, null, null};
	return b;
}

GrammarBuilder gramb_add(GrammarBuilder b, Group g){
	if(!b || !g) return b;
	if(!b->first) b->first = b->last = g;
	else b->last = (b->last->next = g);
	return b;
}

Grammar gramb_uild(GrammarBuilder b){
	if(!b) return null;
	new(Grammar, g);
	size_t grc = 0;
	for(Group gr = b->first; gr; gr = gr->next) grc++;
	*g = (struct grammar){b->name, b->first, grc};
	free(b);
	return g;
}
