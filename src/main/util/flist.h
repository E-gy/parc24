#pragma once

#include "argsarr_mut.h"
#include <parc24/patterns.h>
#include <parc24/options.h>

/**
 * @param pattern @ref 
 * @param opts @copy
 * @return @produces 
 */
ArgsArr_Mut files_list(Pattern pattern, struct parc_options opts);
