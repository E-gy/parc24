#pragma once

#include <ptypes.h>
#include <calp/ast.h>

typedef AST Funcmd;

#ifndef _FUNCSTORE_IMPL
struct funcstore;
typedef struct funcstore *FuncStore;
#endif

/**
 * @returns @produces store
 */
FuncStore funcstore_new(void);

/**
 * @param @consumes store
 */
void funcstore_destroy(FuncStore store);

/**
 * @param @ref store 
 * @return @produces store 
 */
FuncStore funcstore_clone(FuncStore store);

/**
 * @param @ref store
 * @param @ref name
 * @returns @ref f
 */
Funcmd funcstore_get(FuncStore store, string name);

/**
 * @param @refmut store
 * @param @ref name
 * @param @ref f
 */
Result funcstore_add(FuncStore store, string name, Funcmd f);

