#pragma once

#include <ptypes.h>

/**
 * @param f @ref
 * @param str @ref
 * @return Result 
 */
Result fddio_writestr(fd_t f, string str);

/**
 * @param f @ref 
 * @param str @produces
 * @return Result 
 */
Result fddio_readstr(fd_t f, string_mut* str);
