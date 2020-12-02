#include <calp/grammar/fun.h>

#include "internals.h"
#include <calp/util/null.h>
#include <stdlib.h>

static Symbol symbol_destroy(Symbol s){
	if(!s) return null;
	const Symbol next = s->next;
	free(s);
	return next;
}

static Rule rule_destroy(Rule r){
	if(!r) return null;
	const Rule next = r->next;
	for(Symbol s = r->symbols; s; s = symbol_destroy(s));
	free(r);
	return next;
}

static Group group_destroy(Group g){
	if(!g) return null;
	const Group next = g->next;
	for(Rule r = g->rules; r; r = rule_destroy(r));
	free(g);
	return next; 
}

void grammar_destroy(Grammar g){
	if(!g) return;
	for(Group gr = g->groups; gr; gr = group_destroy(gr));
	free(g);
}

#ifndef NDEBUG

#include <calp/util/log.h>

void symbols_logi(Grammar g, Symbol s){
	for(; s; s = s->next){
		switch (s->type){
			case SYMBOL_TYPE_TERM:
				logif("'%s'", s->val.term.name);
				break;
			case SYMBOL_TYPE_GROUP: {
				string grn = null;
				for(Group gg = g->groups; gg && !grn; gg = gg->next) if(gg->id == s->val.group.id) grn = gg->name; 
				logif("<%s>", grn ? grn : "[UNKNOWN GROUP]");
				break;
			}
		}
		if(s->next) logi(" ");
	}
}

void grammar_log(Grammar g){
	if(!g) return;
	logf("%s:", g->name);
	for(Group gg = g->groups; gg; gg = gg->next){
		logf("	%s:", gg->name);
		for(Rule r = gg->rules; r; r = r->next){
			logi("		| ");
			symbols_logi(g, r->symbols);
			log("");
		}
	}
}

#else 

void symbols_logi(Grammar g, Symbol s){}
void grammar_log(Grammar grammar){}

#endif
