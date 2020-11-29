#pragma once

#include <ptypes.h>

#include "var_store.h"
#include "io.h"
#include <cppo/types.h>

struct parcontext {
	/** @ref */ VarStore vars;
	struct exe_opts exeopts;
	ParC24IO io;
};
typedef struct parcontext* ParContext;
