#pragma once

#include <ptypes.h>

#include "var_store.h"
#include "ccmdstore.h"
#include "io.h"
#include <cppo/types.h>
#include <calp/parser.h>

struct parcontext {
	/** @ref */ VarStore vars;
	/** @ref */ CCMDStore ccmds;
	struct exe_opts exeopts;
	ParC24IO io;
	Parser parcer;
};
typedef struct parcontext* ParContext;

/**
 * @param variable @ref 
 * @return @ref value
 */
string parcontext_getunivar(string variable);
