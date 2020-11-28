#include <calp/grammar/fun.h>

#include "internals.h"
#include <calp/util/null.h>

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
