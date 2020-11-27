#pragma once

#include <catch2/catch.hpp>

#define FAIL_FMT(...) do { char* str = (char*) calloc(1024, sizeof(*str)); sprintf(str, __VA_ARGS__); FAIL(str); free(str); } while(0)
#define FAIL_CHECK_FMT(...) do { char* str = (char*) calloc(1024, sizeof(*str)); sprintf(str, __VA_ARGS__); FAIL(str); free(str); } while(0)
