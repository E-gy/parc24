#pragma once

#include <parc24/options.h>
#include <parc24/ioslog.h>

struct tihsopts {
	/** @ref */ string_mut commandstr;
	/** @ref */ string_mut commandfile;
	/** @ref */ argsarr args;
	struct parc_options parcopts;
};
typedef struct tihsopts* TihsOptions;

#define tihsopts_from_stdin(opts) (!(opts).commandstr && !(opts).commandfile)

Result_T(tihsopts_parse_result, struct tihsopts, string_v);
#define TihsOptsParseResult struct tihsopts_parse_result

/**
 * @param args @ref
 * @param io
 * @returns @produces options
 */
TihsOptsParseResult tihsopts_parse(argsarr args, IOsStack io);
#define tihsopts_parse_caste(args, io) tihsopts_parse((argsarr) args, io)
