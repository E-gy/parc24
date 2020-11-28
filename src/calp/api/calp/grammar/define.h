#pragma once

#include "build.h"

#define DEF_SYMBOL_TERMINAL(name, pbody) static string name(string str) pbody

#define DEF_GROUP(name, rules) static Group name(){ GroupBuilder _gb = groupb_new(&name, #name); if(_gb){ rules; } return groupb_uild(_gb); }

#define RULE(symbols) do { RuleBuilder _rb = ruleb_new(); if(!_rb) break; symbols; groupb_add(_gb, ruleb_uild(_rb)); } while(0)
#define SYMBOL_T(name) ruleb_add(_rb, symbol_new_term(&name, #name))
#define SYMBOL_G(name) ruleb_add(_rb, symbol_new_group(&name))

#define DEF_GRAMMAR(name, groups) Grammar name(){ GrammarBuilder _gb = gramb_new(#name); if(_gb){ groups; } return gramb_uild(_gb); }
#define GROUP(name) gramb_add(_gb, name())
