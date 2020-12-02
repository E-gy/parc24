#pragma once

#define null NULL

#define new(_type, _var) _type _var = malloc(sizeof(*_var)); if(!_var) return null;
#define captclean(v, cleanclo) __extension__({ __typeof__ (v) _v = v; cleanclo _v; })
#define retclean(ret, cleanclo) return captclean(ret, cleanclo)
