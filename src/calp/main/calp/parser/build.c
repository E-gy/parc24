#include <calp/parser/build.h>

#include "internals.h"
#include <stdlib.h>

Result_T(pbf_result, void*, string_v);
#define PBFResult struct pbf_result

static PBFResult parser_build_firsts(Grammar gr, EntitiesMap m, EntityInfo gi){
	if(gi->type != SYMB_GROUP) return Error_T(pbf_result, {"Invoked build first on a group"});
	if(gi->i.group.firsts) return Ok_T(pbf_result, null);
	if(!(gi->i.group.firsts = FirstList_new())) return Error_T(pbf_result, {"First list construction failed"});
	Group g = gi->i.group.group;
	for(Rule r = g->rules; r; r = r->next){
		Symbol fnn = r->symbols;
		for(; fnn; fnn = fnn->next){
			EntityInfo fnni = entimap_get(m, entinf_blank_symbol(fnn));
			if(!fnni) return Error_T(pbf_result, {"Invalid state - entity not in map .~."});
			switch(fnni->type){
				case SYMB_TERM:
					if(!fnni->nullable && FirstList_add(gi->i.group.firsts, fnni, r, g->priority + r->priority + fnni->i.term.symbol->priority) != Ok) return Error_T(pbf_result, {"First list add element failed"});
					break;
				case SYMB_GROUP: {
					if(fnni->i.group.group == g){
						fnni->i.group.firsts->lr.r = r;
						fnni->i.group.firsts->lr.from = fnn;
						break;
					}
					PBFResult bfr = parser_build_firsts(gr, m, fnni);
					if(!IsOk_T(bfr)) return bfr;
					for(FirstListElement cpfl = fnni->i.group.firsts->first; cpfl; cpfl = cpfl->next) if(FirstList_add(gi->i.group.firsts, cpfl->symbol, r, g->priority + r->priority + cpfl->priority) != Ok) return Error_T(pbf_result, {"First list add element failed"});
					break;
				}
				default: break;
			}
			if(!fnni->nullable) break;
		}
		EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
		if(ri && ri->nullable) gi->i.group.firsts->fallback = r;
	}
	return Ok_T(pbf_result, null);
}

#define whilechanges(blk) do { bool changes; do { changes = false; blk } while(changes); } while(0)

ParserBuildResult parser_build(Grammar gr){
	//TODO Clean up on errors
	const EntitiesMap m = entimap_new();
	if(!m) return Error_T(parser_build_result, {"Entities Map construction failed"});
	//population & nullable:symbols
	for(Group g = gr->groups; g; g = g->next){
		EntityInfo gi = entimap_add(m, entinf_blank_group(g->id));
		if(!gi) return Error_T(parser_build_result, {"Entities Map add failed"});
		gi->i.group.group = g;
		for(Rule r = g->rules; r; r = r->next){
			if(!entimap_add(m, entinf_blank_rule(r))) return Error_T(parser_build_result, {"Entities Map add failed"});
			for(Symbol s = r->symbols; s; s = s->next){
				EntityInfo si = entimap_add(m, entinf_blank_symbol(s));
				if(!si->init && si->type == SYMB_TERM){
					si->init = true;
					si->nullable = !!si->i.term.symbolId("");
					si->i.term.symbol = s;
				}
			}
		}
	}
	//nullable:rules & nullable:groups
	whilechanges({
		for(Group g = gr->groups; g; g = g->next){
			EntityInfo gi = entimap_get(m, entinf_blank_group(g->id));
			if(!gi) return Error_T(parser_build_result, {"Invalid state - entity not in map .~."});
			if(!gi->init) changes = true;
			bool newgnul = false;
			for(Rule r = g->rules; r; r = r->next){
				EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
				if(!ri) return Error_T(parser_build_result, {"Invalid state - entity not in map .~."});
				if(!ri->init) changes = true;
				bool newrnul = true;
				for(Symbol s = r->symbols; s && newrnul; s = s->next){
					EntityInfo si = entimap_get(m, entinf_blank_symbol(s));
					if(!si) return Error_T(parser_build_result, {"Invalid state - entity not in map .~."});
					if(!si->init) break;
					newrnul &= si->nullable;
				}
				if(newrnul != ri->nullable) changes = (ri->nullable = newrnul) || true;
				ri->init = true;
				newgnul |= ri->nullable;
			}
			if(newgnul != gi->nullable) changes = (gi->nullable = newgnul) || true;
			gi->init = true;
		}
	});
	//first(:groups)
	for(Group g = gr->groups; g; g = g->next){
		EntityInfo gi = entimap_get(m, entinf_blank_group(g->id));
		if(!gi) return Error_T(parser_build_result, {"Invalid state - entity not in map .~."});
		IfError_T(parser_build_firsts(gr, m, gi), err, {
			return Error_T(parser_build_result, err);
		});
	}
	// entimap_log(gr, m);
	Parser p = malloc(sizeof(*p));
	if(!p) return Error_T(parser_build_result, {"Parser allocation failed"});
	*p = (struct parser){gr, m};
	return Ok_T(parser_build_result, p);
}

void parser_destroy(Parser p){
	if(!p) return;
	entimap_destroy(p->ents);
	grammar_destroy(p->grammar);
	free(p);
}
