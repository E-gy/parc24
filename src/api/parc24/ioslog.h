#pragma once

/**
 * @file io.h
 * ParC24 relies entirely on an IO stack.
 * 
 * Mostly everything, including messages by ParC itself respect provided IO stack and will log to std streams as the stack defines them.
 * 
 */

#include <ptypes.h>

#ifndef _IOSSTACK_TDEF
#define _IOSSTACK_TDEF
typedef struct iosstack* IOsStack;
#endif

enum log_level {
	LL_DEBUG = 0, LL_INFO, LL_WARN, LL_ERROR, LL_CRITICAL,
	/** bypasses IO stack, prints to std err */ LL_SUPERCRITICAL
};
typedef enum log_level LogLevel;

IOsStack parcio_new_fromstd(void);

/**
 * Write message (to console)
 * 
 * @param io @ref
 * @param level log level
 * @param str @ref message
 */
void parciolog(IOsStack io, enum log_level level, string str, ...);
/**
 * Same as log, but without the fancy.
 * 
 * Message is print-formatted as is - without newline or level prefix.
 * Levels error and above are printed to error stream.
 * 
 * @param io @ref
 * @param level 
 * @param str @ref
 */
void parcioprintf(IOsStack io, enum log_level level, string str, ...);

Result_T(parc24io_read_result, string_mut, string_v);
#define ParC24IOReadResult struct parc24io_read_result

/**
 * Reads next input
 * 
 * @param io @ref
 * @returns @produces next input, or `null` on EOI
 */
ParC24IOReadResult parcioread(IOsStack io);
