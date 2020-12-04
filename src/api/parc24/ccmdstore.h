#pragma once

#include <ptypes.h>

struct travast_result;
#define TraverseASTResult struct travast_result
typedef struct parcontext* ParContext;

typedef TraverseASTResult (*CCMD)(argsarr args, ParContext context);

struct ccmdstore;
typedef struct ccmdstore* CCMDStore;

/**
 * @return @produces CCMDStore 
 */
CCMDStore ccmdstore_new();

/**
 * @param @consumes store 
 */
void ccmdstore_destroy(CCMDStore store);

/**
 * @param @ref store 
 * @param @ref cmd 
 * @return @ref CCMD 
 */
CCMD ccmdstore_get(CCMDStore store, string cmd);

/**
 * @param @remut store 
 * @param @ref cmd 
 * @param @ref exe 
 * @return Result 
 */
Result ccmdstore_set(CCMDStore store, string cmd, CCMD exe);
