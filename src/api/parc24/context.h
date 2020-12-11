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
typedef struct parcontext* ParContext;

/**
 * @param context @ref
 * @param variable @ref 
 * @return @ref value
 */
string parcontext_getunivar(ParContext context, string variable);
