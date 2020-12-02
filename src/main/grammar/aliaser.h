#pragma once

#include <parc24/aliastore.h>
#include <util/argsarr_mut.h>

Result_T(realias_result, /** @nullable */ ArgsArr_Mut, string_v);
#define RealiasResult struct realias_result

/**
 * @param args @ref
 * @param aliases @ref
 * @return @produces aliased args (`null` if no aliasing is necessary)
 */
RealiasResult realias(string args[], AliasStore aliases);
