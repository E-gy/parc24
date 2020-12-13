#pragma once

#include <ptypes.h>

typedef struct patstate* State;
typedef struct patransition* Transition; 

struct patstate {
	bool accepting;
	Transition transitions;
	/** @ref? */ State defolt;
};
struct patransition {
	char c;
	/** @ref? */ State to;
	Transition next;
};

Transition patransition_new(char c, State to);
Transition patransition_destroy(Transition t);
State patstate_new(bool accepting);
void patstate_destroy(State s);
Result patstate_tradd(State s, Transition t);

typedef State Automaton;

/**
 * @param a @consumes 
 */
void auto_destroy(Automaton a);
/**
 * @param a @refmut 
 */
Result auto_negate(Automaton a);

/**
 * @param a1 @ref 
 * @param a2 @ref 
 * @param aor 
 * @return State 
 */
Automaton auto_merge(Automaton a1, Automaton a2, bool aor);
#define auto_union(a1, a2) auto_merge(a1, a2, true)
#define auto_or auto_union
#define auto_isect(a1, a2) auto_merge(a1, a2, false)
#define auto_and auto_isect

/**
 * @param a1 @ref 
 * @param a2 @ref 
 * @return State 
 */
Automaton auto_concat(Automaton a1, Automaton a2);

/**
 * @param a @ref 
 * @param str @ref
 * @return 
 */
bool auto_test(Automaton a, string str);
