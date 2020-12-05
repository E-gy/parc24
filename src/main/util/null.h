#pragma once

#define null NULL

#define new(_type, _var) _type _var = malloc(sizeof(*_var)); if(!_var) return null;
#define captclean(v, cleanclo) __extension__({ __typeof__ (v) _v = v; cleanclo _v; })
#define retclean(ret, cleanclo) return captclean(ret, cleanclo)

#define ntarrlen(arr) __extension__({ size_t l = 0; for(__typeof__ (arr) _a = arr; _a && *_a; _a++) l++; l; })
