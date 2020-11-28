#pragma once

#include <ptypes.h>

struct var_store;
typedef struct var_store *VarStore;

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
 * @param @ref var
 */
Result varstore_remove(VarStore store, string var);
