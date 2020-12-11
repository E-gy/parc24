#include "parallels.h"

#include <stdlib.h>
#include <util/null.h>
#include <util/thread.h>
#include <util/string.h>
#include <util/fddio.h>
#include <unistd.h>

ThreadResult parallels_runf(void* (fun)(void*), void* arg, bool background){
	pthread_t t;
	if(pthread_create(&t, null, fun, arg) != 0) return Error_T(parallels_tc_result, {"thread create failed"});
	if(background && pthread_detach(t) != 0) return Error_T(parallels_tc_result, {"thread detach failed"});
	return Ok_T(parallels_tc_result, background ? nullthread : t);
}

struct writestri {
	fd_t f;
	string_mut str;
};
typedef struct writestri* WI;

static int writestr(WI i){
	Result ok = fddio_writestr(i->f, i->str);
	if(i->f > 2) close(i->f);
	free(i->str);
	free(i);
	return ok;
}
static threadfwrap_reti(writestr)

ThreadResult parallels_writestr(fd_t f, string str, bool background){
	if(f < 0 || !str) return Error_T(parallels_tc_result, {"invalid args"});
	pthread_t t;
	WI i = malloc(sizeof(*i));
	string_mut strd = strdup(str);
	if(!i) retclean(Error_T(parallels_tc_result, {"thread args transfer failed"}), { free(i); free(strd); });
	*i = (struct writestri){f, strd};
	return parallels_runf(writestr_wrap, i, background);
}

struct readstri {
	fd_t f;
	string_mut* str;
};
typedef struct readstri* RI;

static int readstr(RI i){
	Result ok = fddio_readstr(i->f, i->str);
	if(i->f > 2) close(i->f);
	free(i);
	return ok;
}
static threadfwrap_reti(readstr)

ThreadResult parallels_readstr(fd_t f, string_mut* str){
	if(f < 0 || !str) return Error_T(parallels_tc_result, {"invalid args"});
	pthread_t t;
	RI i = malloc(sizeof(*i));
	if(!i) return Error_T(parallels_tc_result, {"thread args transfer failed"});
	*i = (struct readstri){f, str};
	return parallels_runf(readstr_wrap, i, false);
}

ThreadWaitResult thread_waitret(thread_t t){
	void* r;
	if(pthread_join(t, &r) != 0) return Error_T(thrwait_result, {"failed to join"});
	return Ok_T(thrwait_result, r);
}
