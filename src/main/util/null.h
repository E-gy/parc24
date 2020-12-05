#pragma once

#define null NULL

#define new(_type, _var) _type _var = malloc(sizeof(*_var)); if(!_var) return null;
#define captclean(v, cleanclo) __extension__({ __typeof__ (v) _v = v; cleanclo _v; })
#define retclean(ret, cleanclo) return captclean(ret, cleanclo)

#define ntarrlen_t(arr, intt) __extension__({ intt l = 0; for(__typeof__ (arr) _a = arr; _a && *_a; _a++) l++; l; })
#define ntarrlen(arr) ntarrlen_t(arr, size_t)
