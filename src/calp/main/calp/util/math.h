#pragma once

#define min(a, b) __extension__({ __typeof__ (a) _a = a; __typeof__ (b) _b = b; _a < _b ? _a : _b; })
#define max(a, b) __extension__({ __typeof__ (a) _a = a; __typeof__ (b) _b = b; _a > _b ? _a : _b; })
