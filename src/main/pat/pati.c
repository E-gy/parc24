#include "pati.h"

#include <util/null.h>
#include <stdlib.h>

Transition patransition_new(char c, State to){
	if(!to) return null;
	new(Transition, t);
	*t = (struct patransition){c, to, null};
	return t;
}

Transition patransition_destroy(Transition t){
	if(!t) return null;
	Transition next = t->next;
	free(t);
	return next;
}

State patstate_new(bool accepting){
	new(State, s);
	*s = (struct patstate){accepting, null, null};
	return s;
}

void patstate_destroy(State s){
	if(!s) return;
	for(Transition t = s->transitions; t; t = patransition_destroy(t));
	free(s);
}

Result patstate_tradd(State s, Transition t){
	if(!s || !t) return Error;
	Transition* i = &s->transitions;
	for(; *i && (*i)->c < t->c; i = &((*i)->next));
	if(*i && (*i)->c == t->c) return Error;
	t->next = *i;
	*i = t;
	return Ok;
}

typedef struct encounter* Encounter;
struct encounter {
	State s;
	Encounter next;
};

static Encounter encounter_new(State s, Encounter next){
	new(Encounter, e);
	*e = (struct encounter){s, next};
	return e;
}

static Encounter encounter_destroy(Encounter e){
	if(!e) return null;
	Encounter next = e->next;
	free(e);
	return next;
}

static void auto_destroy_(State s, Encounter* es){
	if(!s) return;
	for(Encounter e = *es; e; e = e->next) if(e->s == s) return;
	*es = encounter_new(s, *es);
	for(Transition t = s->transitions; t; t = t->next) auto_destroy_(t->to, es);
	auto_destroy_(s->defolt, es);
	patstate_destroy(s);
}

void auto_destroy(State a){
	if(!a) return;
	Encounter es = null;
	auto_destroy_(a, &es);
	for(; es; es = encounter_destroy(es));
}

static Result auto_negate_(State s, Encounter* es, State* sinka){
	if(!s) return Ok;
	for(Encounter e = *es; e; e = e->next) if(e->s == s) return Ok;
	Encounter e = encounter_new(s, *es);
	if(!e) return Error;
	*es = e;
	s->accepting = !s->accepting;
	for(Transition t = s->transitions; t; t = t->next) if(!IsOk(auto_negate_(t->to, es, sinka))) return Error;
	if(s->defolt){
		if(!IsOk(auto_negate_(s->defolt, es, sinka))) return Error;
	} else {
		if(!*sinka){
			if(!(*sinka = patstate_new(true))) return Error;;
			(*sinka)->defolt = *sinka;
		}
		s->defolt = *sinka;
	}
	return Ok;
}

Result auto_negate(State a){
	if(!a) return Error;
	Encounter es = null;
	State sinka = null;
	Result nr = auto_negate_(a, &es, &sinka);
	for(; es; es = encounter_destroy(es));
	return nr;
}

typedef struct amerger* Merger;
struct amerger {
	/** @ref */ State sr;
	Merger next;
	/** @ref */ State s[];
};

/**
 * @param sr @ref
 * @return @produces Merger 
 */
static Merger merger_new(State sr, size_t msc){
	Merger m = malloc(sizeof(*m) + msc*sizeof(State));
	if(!m) return null;
	*m = (struct amerger){sr, null};
	return m;
}

static Merger merger_new2(State s1, State s2, State sr){
	Merger m = merger_new(sr, 2);
	if(!m) return null;
	m->s[0] = s1;
	m->s[1] = s2;
	return m;
}

/**
 * @param m @consumes 
 * @return @produces Merger 
 */
static Merger merger_destroy(Merger m){
	if(!m) return null;
	Merger next = m->next;
	free(m);
	return next;
}

static State auto_merge_(State s1, State s2, Merger* mergers, bool aor){
	if(!s1 && !s2) return null;
	for(Merger m = *mergers; m; m = m->next) if(m->s[0] == s1 && m->s[1] == s2) return m->sr;
	State merged = patstate_new(aor ? (s1 && s1->accepting) || (s2 && s2->accepting) : (s1 && s1->accepting) && (s2 && s2->accepting));
	if(!merged) return null;
	#define cleanup {patstate_destroy(merged);}
	Merger merger = merger_new2(s1, s2, merged);
	if(!merger) retclean(null, cleanup);
	merger->next = *mergers;
	*mergers = merger;
	Transition t1 = s1 ? s1->transitions : null;
	Transition t2 = s2 ? s2->transitions : null;
	Transition* t3 = &merged->transitions;
	for(; t1 && t2; t3 = &((*t3)->next)){
		if(t1->c == t2->c){
			*t3 = patransition_new(t1->c, auto_merge_(t1->to, t2->to, mergers, aor));
			t1 = t1->next;
			t2 = t2->next;
		} else if(t1->c < t2->c){
			*t3 = patransition_new(t1->c, auto_merge_(t1->to, s2 ? s2->defolt : null, mergers, aor));
			t1 = t1->next;
		} else {
			*t3 = patransition_new(t2->c, auto_merge_(s1 ? s1->defolt : null, t2->to, mergers, aor));
			t2 = t2->next;
		}
		if(!*t3) retclean(null, cleanup);
	}
	for(; t1; t1 = t1->next, t3 = &((*t3)->next)) if(!(*t3 = patransition_new(t1->c, auto_merge_(t1->to, s2 ? s2->defolt : null, mergers, aor)))) retclean(null, cleanup);
	for(; t2; t2 = t2->next, t3 = &((*t3)->next)) if(!(*t3 = patransition_new(t2->c, auto_merge_(s1 ? s1->defolt : null, t2->to, mergers, aor)))) retclean(null, cleanup);
	if((s1 && s1->defolt) || (s2 && s2->defolt)) if(!(merged->defolt = auto_merge_(s1 ? s1->defolt : null, s2 ? s2->defolt : null, mergers, aor))) retclean(null, cleanup);
	return merged;
}

State auto_merge(State a1, State a2, bool aor){
	if(!a1) return a2;
	if(!a2) return a1;
	Merger mergers = null;
	State merged = auto_merge_(a1, a2, &mergers, aor);
	for(; mergers; mergers = merger_destroy(mergers));
	return merged;
}

bool auto_test(State a, string str){
	if(!a || !str) return false;
	if(!*str) return a->accepting;
	const char c = *str;
	for(Transition t = a->transitions; t && t->c <= c; t = t->next) if(t->c == c) return auto_test(t->to, str+1);
	return auto_test(a->defolt, str+1);
}
