#pragma once

/**
 * @file define.h
 * Grammar compile time syntatic definition.
 * 
 * Provides tools necessary to "construct" grammar at compile time.
 * 
 */

#include "build.h"

#define DEF_SYMBOL_TERMINAL(name, pbody) static string name(string str) pbody

#define DEF_GROUPp(name, priority, rules) static Group name(){ GroupBuilder _gb = groupb_new(&name, #name, priority); if(_gb){ rules; } return groupb_uild(_gb); }
#define DEF_GROUP(name, rules) DEF_GROUPp(name, 0, rules)

#define RULEp(priority, symbols) do { RuleBuilder _rb = ruleb_new(priority); if(!_rb) break; symbols; groupb_add(_gb, ruleb_uild(_rb)); } while(0)
#define RULE(symbols) RULEp(0, symbols)
#define SYMBOL_Tp(name, priority) ruleb_add(_rb, symbol_new_term(&name, #name, priority))
#define SYMBOL_Gp(name, priority) ruleb_add(_rb, symbol_new_group(&name, priority))
#define SYMBOL_T(name) SYMBOL_Tp(name, 0)
#define SYMBOL_G(name) SYMBOL_Gp(name, 0)

#define DEF_GRAMMAR(name, groups) Grammar name(){ GrammarBuilder _gb = gramb_new(#name); if(_gb){ groups; } return gramb_uild(_gb); }
#define GROUP(name) gramb_add(_gb, name())
