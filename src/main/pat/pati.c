#include "pati.h"

#include <util/null.h>
#include <util/string.h>
#include <util/caste.h>
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
			if(!(*sinka = patstate_new(true))) return Error;
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
	size_t sc;
	/** @ref */ State s[];
};

/**
 * @param sr @ref
 * @return @produces Merger 
 */
static Merger merger_new(State sr, size_t msc){
	Merger m = malloc(sizeof(*m) + msc*sizeof(State));
	if(!m) return null;
	*m = (struct amerger){sr, null, msc};
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

static Merger mergers_find(Merger* mergers, Merger liek){
	if(!mergers || !liek) return null;
	for(Merger m = *mergers; m; m = m->next) if(m->sc == liek->sc && !memcmp(m->s, liek->s, m->sc*sizeof(State))) return m;
	return null;
}

/**
 * @param m @consumes @produces
 * @param as @ref
 * @return whether the state was added
 */
static bool merger_adds(Merger* mm, State as){
	if(!mm || !*mm || !as) return false;
	const Merger m = *mm;
	size_t i = 0;
	for(; i < m->sc && ptr2ull(m->s[i]) < ptr2ull(as); i++);
	if(i < m->sc && m->s[i] == as) return false;
	Merger nm = realloc(m, sizeof(*nm) + (m->sc+1)*(sizeof(State)));
	if(!nm) return false;
	memmove(nm->s+i+1, nm->s+i, (nm->sc-i)*sizeof(State));
	nm->sc++;
	nm->s[i] = as;
	*mm = nm;
	return true;
}

typedef struct epstrl* EpsTransitionList;
struct epstrl {
	State from;
	State to;
	EpsTransitionList next;
};

/**
 * @param list @consumes 
 * @param from @ref
 * @param to @ref @nullable
 * @return @produces list 
 */
static EpsTransitionList epstrl_add(EpsTransitionList list, State from, State to){
	if(!from) return list;
	new(EpsTransitionList, e);
	*e = (struct epstrl){from, to, list};
	return e;
}

/**
 * @param l @consumes 
 */
static void epstrl_destroy(EpsTransitionList l){
	if(!l) return;
	epstrl_destroy(l->next);
	free(l);
}

/**
 * @param a @ref 
 * @param eps @ref
 * @param merger @consumes
 * @param mergers @refmut
 * @return @produces 
 */
static State auto_addeps_(State a, EpsTransitionList eps, Merger merger, Merger* mergers){
	while(true){
		bool changed = false;
		for(EpsTransitionList e = eps; e; e = e->next) for(size_t i = 0; i < merger->sc; i++) if(e->to && e->from == merger->s[i] && merger_adds(&merger, e->to)){ changed = true; break; }
		if(!changed) break;
	}
	if(!merger) return null;
	{
		Merger m = mergers_find(mergers, merger);
		if(m){
			merger_destroy(merger);
			return m->sr;
		}
	}
	merger->next = *mergers;
	*mergers = merger;
	bool accepting = false;
	for(EpsTransitionList e = eps; e && !accepting; e = e->next) for(size_t i = 0; i < merger->sc && !accepting; i++) accepting |= !e->to && e->from == merger->s[i];
	const State merged = patstate_new(accepting);
	if(!merged) return null;
	merger->sr = merged;
	Transition* ts = calloc(merger->sc, sizeof(*ts));
	if(!ts) retclean(null, {patstate_destroy(merged);});
	for(size_t i = 0; i < merger->sc; i++) ts[i] = merger->s[i]->transitions;
	Transition* tr = &merged->transitions;
	for(unsigned char c = 0; c < 128; c++){
		Merger nss = merger_new(null, 0);
		for(size_t i = 0; i < merger->sc; i++) if(ts[i] && ts[i]->c == c) merger_adds(&nss, ts[i]->to);
		if(nss->sc == 0){
			merger_destroy(nss);
			continue;
		}
		for(size_t i = 0; i < merger->sc; i++) if(ts[i] && ts[i]->c == c) ts[i] = ts[i]->next; else if(merger->s[i]->defolt) merger_adds(&nss, merger->s[i]->defolt);
		if(!(*tr = patransition_new(c, auto_addeps_(a, eps, nss, mergers)))) retclean(null, { merger_destroy(nss); free(ts); patstate_destroy(merged); });
		tr = &((*tr)->next);
	}
	{
		Merger dsm = merger_new(null, 0);
		for(size_t i = 0; i < merger->sc; i++) if(merger->s[i]->defolt) merger_adds(&dsm, merger->s[i]->defolt);
		if(dsm->sc > 0) merged->defolt = auto_addeps_(a, eps, dsm, mergers); else merger_destroy(dsm);
	}
	free(ts);
	return merged;
}

/**
 * @param a @ref 
 * @param eps @ref
 * @return @produces 
 */
State auto_addeps(State a, EpsTransitionList eps){
	if(!a) return null;
	Merger mergers = null;
	Merger m0 = merger_new(null, 1);
	if(!m0) return null;
	m0->s[0] = a;
	State concatenated = auto_addeps_(a, eps, m0, &mergers);
	for(; mergers; mergers = merger_destroy(mergers));
	return concatenated;
}

static Result auto_concat_ceps1(State s, State a2, EpsTransitionList* eps, Encounter* es){
	if(!s) return Ok;
	for(Encounter e = *es; e; e = e->next) if(e->s == s) return Ok;
	*es = encounter_new(s, *es);
	if(s->accepting){
		EpsTransitionList neps = epstrl_add(*eps, s, a2);
		if(!neps) return Error;
		*eps = neps;
	}
	for(Transition t = s->transitions; t; t = t->next) if(!IsOk(auto_concat_ceps1(t->to, a2, eps, es))) return Error;
	if(s->defolt && !IsOk(auto_concat_ceps1(s->defolt, a2, eps, es))) return Error;
	return Ok;
}

static Result auto_concat_ceps2(State s, EpsTransitionList* eps, Encounter* es){
	if(!s) return Ok;
	for(Encounter e = *es; e; e = e->next) if(e->s == s) return Ok;
	*es = encounter_new(s, *es);
	if(s->accepting){
		EpsTransitionList neps = epstrl_add(*eps, s, null);
		if(!neps) return Error;
		*eps = neps;
	}
	for(Transition t = s->transitions; t; t = t->next) if(!IsOk(auto_concat_ceps2(t->to, eps, es))) return Error;
	if(s->defolt && !IsOk(auto_concat_ceps2(s->defolt, eps, es))) return Error;
	return Ok;
}

State auto_concat(State a1, State a2){
	if(!a1) return a2;
	if(!a2) return a1;
	State concatenated = null;
	EpsTransitionList eps = null;
	Encounter es1 = null;
	Encounter es2 = null;
	if(IsOk(captclean(auto_concat_ceps1(a1, a2, &eps, &es1), {for(; es1; es1 = encounter_destroy(es1));}))) if(IsOk(captclean(auto_concat_ceps2(a2, &eps, &es2), {for(; es2; es2 = encounter_destroy(es2));}))) concatenated = auto_addeps(a1, eps);
	epstrl_destroy(eps);
	return concatenated;
}

bool auto_test(State a, string str){
	if(!a || !str) return false;
	if(!*str) return a->accepting;
	const char c = *str;
	for(Transition t = a->transitions; t && t->c <= c; t = t->next) if(t->c == c) return auto_test(t->to, str+1);
	return auto_test(a->defolt, str+1);
}
