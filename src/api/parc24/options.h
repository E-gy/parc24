#pragma once

#include <ptypes.h>

struct parc_options {
	bool dotglob;
	bool extglob;
	bool nocaseglob;
	bool nullglob;
	bool expand_aliases;
	bool sourcepath;
	bool xpg_echo;
};
typedef struct parc_options* ParcOptions;

#define parc_options_default ((struct parc_options){.expand_aliases = true, .sourcepath = true})
