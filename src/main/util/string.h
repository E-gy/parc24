#pragma once

#include <string.h>
#include <ptypes.h>

#define streq !strcmp
#define strpref(prefix, str) (!strncmp(prefix, str, strlen(prefix)))
