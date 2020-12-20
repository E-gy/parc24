#pragma once

#include <ptypes.h>

#include "var_store.h"
#include "funcstore.h"
#include "ccmdstore.h"
#include "aliastore.h"
#include "iosstack.h"
#include "wdstek.h"
#include "options.h"
#include "patterns.h"
#include "arith.h"
#include <cppo/types.h>
#include <calp/parser.h>

struct parcontext {
	/** @ref */ VarStore vars;
	/** @ref */ VarStore vars_cmus;
	/** @ref */ FuncStore funcs;
	/** @ref */ CCMDStore ccmds;
	/** @ref */ AliasStore aliases;
	/** @ref */ IOsStack ios;
	/** @ref */ WorkDirStack wd;
	/** @ref */ PatternCompiler patcomp;
	/** @ref */ Arithmetics arith;
	string currexe;
	argsarr args;
	int lastexit;
	int lupdepth;
	bool exeback;
	ParcOptions parcopts;
	Parser parcer;
};
#ifndef _PARCONTEXT_TDEF
#define _PARCONTEXT_TDEF
typedef struct parcontext* ParContext;
#endif

struct getvarv {
	bool copy;
	union {
		/** @ref */ string ref;
		string_mut copy;
	} v;
};

/**
 * 
 * Dups `vars`, `funcs`, `aliases`, `wd`, and `parcopts`.
 * 
 * @param context @refmut 
 * @return Result 
 */
Result parcontext_subco_all(ParContext context);

/**
 * Destroys `vars`, `funcs`, `aliases`, `wd`, and `parcopts`.
 * 
 * @param context @refmut 
 */
void parcontext_subco_destroy(ParContext context);

/**
 * @param context @ref
 * @param variable @ref 
 * @return @produces value
 */
struct getvarv parcontext_getunivar(ParContext context, string variable);
