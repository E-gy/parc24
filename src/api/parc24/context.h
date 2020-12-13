#pragma once

#include <ptypes.h>

#include "var_store.h"
#include "funcstore.h"
#include "ccmdstore.h"
#include "aliastore.h"
#include "iosstack.h"
#include "options.h"
#include "patterns.h"
#include <cppo/types.h>
#include <calp/parser.h>

struct parcontext {
	/** @ref */ VarStore vars;
	/** @ref */ FuncStore funcs;
	/** @ref */ CCMDStore ccmds;
	/** @ref */ AliasStore aliases;
	/** @ref */ IOsStack ios;
	/** @ref */ PatternCompiler patcomp;
	string currexe;
	argsarr args;
	int lastexit;
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
 * @param context @ref
 * @param variable @ref 
 * @return @produces value
 */
struct getvarv parcontext_getunivar(ParContext context, string variable);
