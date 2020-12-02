#pragma once

#include <ptypes.h>

#ifndef _ALIASSTORE_IMPL
struct aliastore;
typedef struct aliastore *AliasStore;
#endif

/**
 * @returns @produces store
 */
AliasStore aliastore_new(void);

/**
 * @param @consumes store
 */
void aliastore_destroy(AliasStore store);

/**
 * @param @ref store 
 * @return @produces stpre 
 */
AliasStore aliastore_clone(AliasStore store);

/**
 * @param @ref store
 * @param @ref a
 * @returns @ref val
 */
string_mut aliastore_get(AliasStore store, string a);

/**
 * @param @refmut store
 * @param @ref a
 * @param @ref val
 */
Result aliastore_add(AliasStore store, string a, string val);

/**
 * @param @refmut store
 * @param @ref a
 */
Result aliastore_remove(AliasStore store, string a);
