#pragma once

#include "null.h"
#include "caste.h"
#include <pthread.h>

#define nullthread 0
#define threadfwrap(f) void* f##_wrap(void* arg){ f(arg); return null; }
#define threadfwrap_retp(f) void* f##_wrap(void* arg){ return f(arg); }
#define threadfwrap_reti(f) void* f##_wrap(void* arg){ return int2ptr(f(arg)); }
