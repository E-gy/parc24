#pragma once

#include <string.h>
#include <ptypes.h>

#define streq !strcmp
#define strpref(prefix, str) (!strncmp(prefix, str, strlen(prefix)))

string_mut strdup(string src);
string_mut strndup(string src, size_t n);
