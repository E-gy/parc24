#pragma once

#include <string.h>
#include <calp/ptypes.h>

#define streq !strcmp

string_mut strdup(string src);
string_mut strndup(string src, size_t n);
