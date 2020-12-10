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

static void auto_negate_(State s, Encounter* es){
	if(!s) return;
	for(Encounter e = *es; e; e = e->next) if(e->s == s) return;
	*es = encounter_new(s, *es);
	s->accepting = !s->accepting;
	for(Transition t = s->transitions; t; t = t->next) auto_negate_(t->to, es);
	auto_negate(s->defolt);
}

void auto_negate(State a){
	if(!a) return;
	Encounter es = null;
	auto_negate_(a, &es);
	for(; es; es = encounter_destroy(es));
}

typedef struct amerger* Merger;
struct amerger {
	/** @ref */ State s1;
	/** @ref */ State s2;
	/** @ref */ State sr;
	Merger next;
};

/**
 * @param s1 @ref 
 * @param s2 @ref
 * @param sr @ref
 * @return @produces Merger 
 */
static Merger merger_new(State s1, State s2, State sr){
	new(Merger, m);
	*m = (struct amerger){s1, s2, sr, null};
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
	for(Merger m = *mergers; m; m = m->next) if(m->s1 == s1 && m->s2 == s2) return m->sr;
	State merged = patstate_new(aor ? s1->accepting || s2->accepting : s1->accepting && s2->accepting);
	if(!merged) return null;
	Merger merger = merger_new(s1, s2, merged);
	if(!merger) retclean(null, {patstate_destroy(merged);});
	merger->next = *mergers;
	*mergers = merger;
	#define cleanup {patransition_destroy(t3);patstate_destroy(merged);}
	Transition t1 = s1->transitions;
	Transition t2 = s2->transitions;
	while(t1 && t2){
		Transition t3;
		if(t1->c == t2->c){
			t3 = patransition_new(t1->c, auto_merge_(t1->to, t2->to, mergers, aor));
			t1 = t1->next;
			t2 = t2->next;
		} else if(t1->c < t2->c){
			t3 = patransition_new(t1->c, auto_merge_(t1->to, s2->defolt, mergers, aor));
			t1 = t1->next;
		} else {
			t3 = patransition_new(t2->c, auto_merge_(s1->defolt, t2->to, mergers, aor));
			t2 = t2->next;
		}
		if(!IsOk(patstate_tradd(merged, t3))) retclean(null, cleanup);
	}
	for(; t1; t1 = t1->next){
		Transition t3 = patransition_new(t1->c, auto_merge_(t1->to, s2->defolt, mergers, aor));
		if(!IsOk(patstate_tradd(merged, t3))) retclean(null, cleanup);
	}
	for(; t2; t2 = t2->next){
		Transition t3 = patransition_new(t2->c, auto_merge_(s1->defolt, t2->to, mergers, aor));
		if(!IsOk(patstate_tradd(merged, t3))) retclean(null, cleanup);
	}
	if(s1->defolt || s2->defolt) if(!(merged->defolt = auto_merge_(s1->defolt, s2->defolt, mergers, aor))) retclean(null, {patstate_destroy(merged);});
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
