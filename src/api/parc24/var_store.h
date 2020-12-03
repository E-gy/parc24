#pragma once

#include <ptypes.h>

#ifndef _VARSTORE_IMPL
struct var_store;
typedef struct var_store *VarStore;
#endif

/**
 * @returns @produces store
 */
VarStore varstore_new(void);

/**
 * @param @consumes store
 */
void varstore_destroy(VarStore store);

/**
 * @param @ref store 
 * @return @produces stpre 
 */
VarStore varstore_clone(VarStore store);

/**
 * @param @ref store
 * @param @ref var
 * @returns @ref val
 */
string_mut varstore_get(VarStore store, string var);

/**
 * @param @refmut store
 * @param @ref var
 * @param @ref val
 */
Result varstore_add(VarStore store, string var, string val);

/**
 * @param @refmut store
 * @param @consumes var _on success_
 * @param @ref val
 */
Result varstore_add_(VarStore store, string var, string_mut val);

/**
 * @param @refmut store
 * @param @ref var
 */
Result varstore_remove(VarStore store, string var);
