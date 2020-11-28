#include "internals.h"

#include <stdlib.h>
#include <calp/util/log.h>
#include <calp/util/string.h>

// First List

FirstList FirstList_new(){
	new(FirstList, l);
	*l = (struct groupfl){null, null, {null, null}};
	return l;
}

Result FirstList_add(FirstList l, EntityInfo symbol, Rule r){
	if(!l) return Error;
	FirstListElement e = malloc(sizeof(*e));
	if(!e) return Error;
	*e = (struct groupfle){symbol, r, l->first};
	l->first = e;
	return Ok;
}

// Entity Info

typedef size_t hash_t;

static hash_t ghash(void* p){
	const char* pc = p;
	const char* p0 = p;
	const size_t red = pc-p0;
	hash_t h = 0;
	for(size_t b = 0; b < sizeof(size_t); b++) h ^= (red>>(b*8ULL))&0xFFULL;
	return h;
}

static hash_t entinf_hash(struct entinf i){
	switch(i.type){
		case SYMB_TERM: return ghash(i.i.term.symbol);
		case SYMB_GROUP: return ghash(i.i.group.group);
		case RULE: return ghash(i.i.rule.rule);
		default: return 0;
	}
}

bool entinf_eq(EntityInfo i, struct entinf ii){
	if(!i) return false;
	if(i->type != ii.type) return false;
	switch(i->type){
		case SYMB_TERM: return i->i.term.symbolId == ii.i.term.symbolId;
		case SYMB_GROUP: return i->i.group.groupId == ii.i.group.groupId;
		case RULE: return i->i.rule.rule == ii.i.rule.rule;
		default: return false;
	}
}

EntitiesMap entimap_new(){
	new(EntitiesMap, m);
	memset(m->ents, 0, ENTIMAPS*sizeof(*m->ents));
	return m;
}

EntityInfo entimap_get(EntitiesMap m, struct entinf ii){
	if(!m) return null;
	const hash_t h = entinf_hash(ii);
	for(EntityInfo i = m->ents[h]; i; i = i->mapnext) if(entinf_eq(i, ii)) return i;
	return null;
}

EntityInfo entimap_add(EntitiesMap m, struct entinf ii){
	if(!m) return null;
	const hash_t h = entinf_hash(ii);
	EntityInfo* i = &m->ents[h];
	for(; *i; i = &((*i)->mapnext)) if(entinf_eq(*i, ii)) return *i;
	new(EntityInfo, nii);
	*nii = ii;
	return *i = nii;
}

#ifndef NDEBUG

#include <calp/util/log.h>
#include <calp/grammar/fun.h>

void entimap_log(Grammar gr, EntitiesMap m){
	logf("%s:", gr->name);
	for(size_t j = 0; j < ENTIMAPS; j++) for(EntityInfo i = m->ents[j]; i; i = i->mapnext) if(i->type == SYMB_TERM){
		logf("	'%s':", i->i.term.symbol->val.term.name);
		logf("		init: %s", i->init ? "true" : "false");
		logf("		nullable: %s", i->nullable ? "true" : "false");
	}
	for(size_t j = 0; j < ENTIMAPS; j++) for(EntityInfo i = m->ents[j]; i; i = i->mapnext) if(i->type == SYMB_GROUP){
		logf("	<%s>:", i->i.group.group->name);
		logf("		init: %s", i->init ? "true" : "false");
		logf("		nullable: %s", i->nullable ? "true" : "false");
		log("		firsts: ");
		for(FirstListElement f = i->i.group.firsts->first; f; f = f->next) logf("			'%s' -> %p", f->symbol->i.term.symbol->val.term.name, f->r);
		logf("		fallback: %p", i->i.group.firsts->fallback);
		if(i->i.group.firsts->lr.r) logf("		lr on: %p", i->i.group.firsts->lr.r);
		log("		rules:");
		for(Rule r = i->i.group.group->rules; r; r = r->next){
			logif("			%p: ", r);
			EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
			if(!ri){
				log("<UNKNOWN RULE>");
				continue;
			}
			symbols_logi(gr, r->symbols);
			log("");
			logf("				init: %s", ri->init ? "true" : "false");
			logf("				nullable: %s", ri->nullable ? "true" : "false");
		}
	}
}

#else

void entimap_log(Grammar gr, EntitiesMap m){}

#endif
