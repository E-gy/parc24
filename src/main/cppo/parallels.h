#pragma once

#include <ptypes.h>

Result_T(parallels_tc_result, thread_t, string_v);
#define ThreadResult struct parallels_tc_result

/**
 * @param f @consumes
 * @param str @ref
 * @param background when set, detaches the thread and returns `nullthread`
 * @return thread that runs the operation
 */
ThreadResult parallels_writestr(fd_t f, string str, bool background);

/**
 * @param f @consumes 
 * @param str @produces
 * @return thread that runs the operation
 */
ThreadResult parallels_readstr(fd_t f, string_mut* str);

Result_T(thrwait_result, void*, string_v);
#define ThreadWaitResult struct thrwait_result

/**
 * @param thread
 */
ThreadWaitResult thread_waitret(thread_t thread);
