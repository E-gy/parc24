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

/**
 * @param a @consumes 
 */
void auto_destroy(State a);
/**
 * @param a @refmut 
 */
Result auto_negate(State a);

/**
 * @param a1 @ref 
 * @param a2 @ref 
 * @param aor 
 * @return State 
 */
State auto_merge(State a1, State a2, bool aor);

/**
 * @param a @ref 
 * @param str @ref
 * @return 
 */
bool auto_test(State a, string str);
