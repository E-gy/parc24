#include <cppo.h>
#include "parallels.h"
#include <util/caste.h>

ExeWaitResult exethread_waitretcode(thread_t t){
	ThreadWaitResult r = thread_waitret(t);
	return IsOk_T(r) ? Ok_T(exewait_result, ptr2int(r.r.ok)) : Error_T(exewait_result, r.r.error);
}
