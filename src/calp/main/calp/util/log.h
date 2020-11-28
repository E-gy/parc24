#pragma once

#include <stdio.h>

#define flushlogs() do { fflush(stdout); fflush(stderr); } while (0)
#define log(str) do { fprintf(stdout, str "\n"); /*fflush(stdout);*/ } while (0)
#define logf(fmt, ...) do { fprintf(stdout, fmt "\n", ##__VA_ARGS__); /*fflush(stdout);*/ } while (0)
#define logi(str) do { fprintf(stdout, str); /*fflush(stdout);*/ } while (0)
#define logif(fmt, ...) do { fprintf(stdout, fmt, ##__VA_ARGS__); /*fflush(stdout);*/ } while (0)
#define logerr(str) do { fprintf(stderr, str "\n"); fflush(stderr); } while (0)
#define logerrf(fmt, ...) do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); fflush(stderr); } while (0)

#ifndef NDEBUG
#define logdebug(fmt, ...) do { printf("[%s:%d]%s: ", __FILE__, __LINE__, __func__); printf(fmt "\n", ##__VA_ARGS__); } while (0)
#else
#define logdebug(...) do {} while (0)
#endif
