#pragma once

#include <calp/grammar.h>

struct symbol {
	enum {
		SYMBOL_TYPE_TERM = 0,
		SYMBOL_TYPE_GROUP,
	} type;
	union {
		struct {
			/** @ref */ TerminalSymbolId id;
			/** @ref */  string name;
		} term;
		struct {
			/** @ref */ GroupId id;
		} group;
	} val;
	Symbol next;
};

struct rule {
	Symbol symbols;
	size_t symbolsc;
	Rule next;
};

struct group {
	/** @ref */ GroupId id;
	/** @ref */ string name;
	Rule rules;
	size_t rulesc;
	Group next;
};

struct grammar {
	/** @ref */ string name;
	Group groups;
	size_t groupsc;
};
