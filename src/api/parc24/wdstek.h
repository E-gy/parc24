#pragma once

#include <ptypes.h>

typedef struct workdirstack* WorkDirStack;

/**
 * @return @produces 
 */
WorkDirStack wdstack_new(void);

/**
 * @param s @consumes 
 */
void wdstack_destroy(WorkDirStack s);

/**
 * @param s @ref 
 * @param depth 
 * @return @nullable string 
 */
string wdstack_get(WorkDirStack s, size_t depth);

/**
 * @param s @refmut 
 * @param dir @ref
 * @return Result 
 */
Result wdstack_changedir(WorkDirStack s, string dir);

/**
 * @param s @refmut 
 * @return Result 
 */
Result wdstack_goback(WorkDirStack s);
