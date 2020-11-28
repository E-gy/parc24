#pragma once

/**
 * @file io.h
 * ParC24 relies on simulated IO.
 * 
 * All reading/writing is performed using implementations of functions defined here.
 * Some internal debug messages _can_ be printed to stdout sometimes (but the goal is to reduce them as much as possible).
 * 
 * At the time of the writing, default behaviour of executed processes concerning their standard IO is not defined.
 * 
 */

#include <ptypes.h>

enum log_level {
	LL_DEBUG = 0, LL_INFO, LL_WARN, LL_ERROR, LL_CRITICAL,
};
typedef enum log_level LogLevel;

/**
 * Write message (to console)
 * 
 * @param level log level
 * @param str @ref message
 */
typedef void (*parc24io_logger)(enum log_level level, string str, ...);

Result_T(parc24io_read_result, string_mut, string_v);
#define ParC24IOReadResult struct parc24io_read_result

/**
 * Reads next input
 * 
 * @returns @produces next input, or `null` on EOI
 */
typedef ParC24IOReadResult (*parc24io_reader)(void);

struct parc24io {
	parc24io_logger log;
	parc24io_reader readline;
};
#define ParC24IO struct parc24io

/**
 * IO that reads/writes from/to std streams.
 * 
 * @return ParC24IO 
 */
ParC24IO parc24io_fromstd();
