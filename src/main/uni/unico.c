#include <parc24/travast.h>

#include <cppo.h>

TraverseASTResult parcontext_uniwait(TraverseASTResult r){
	if(!IsOk_T(r)) return r;
	switch(r.r.ok.type){
		case TRAV_WAIT_CHILD: {
			ExeWaitResult cw = exe_waitretcode(r.r.ok.v.child);
			return IsOk_T(cw) ? Ok_T(travast_result, {TRAV_COMPLETED, {.completed = cw.r.ok}}) : Error_T(travast_result, cw.r.error);
		}
		case TRAV_WAIT_THREAD: {
			ExeWaitResult cw = exethread_waitretcode(r.r.ok.v.thread);
			return IsOk_T(cw) ? Ok_T(travast_result, {TRAV_COMPLETED, {.completed = cw.r.ok}}) : Error_T(travast_result, cw.r.error);
		}
		default: return r;
	}
}
