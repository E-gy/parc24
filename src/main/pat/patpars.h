#pragma once

#include "pati.h"

#define _PATTERN_DEF
typedef Automaton Pattern;

#include <parc24/patterns.h>
#include <calp/parser.h>

struct pattern_compiler {
	Parser standard;
	Parser extended;
};

Parser patpars_new(void);
Parser patpars_ext_new(void);

PatCompResult pattern_parse(Parser parser, string str, bool doublestar);
PatCompResult pattern_parse_ext(Parser parser, string str, bool doublestar);
