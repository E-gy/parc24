#include <parc24/wdstek.h>

#include <util/null.h>
#include <util/string.h>
#include <stdlib.h>
#include <util/buffer.h>
#include <unistd.h>
#include <limits.h>

typedef struct dirhel* HistEl;
struct dirhel {
	string_mut dir;
	HistEl prev;
};

static HistEl dirhel_new(string_mut dir){
	if(!dir) return null;
	new(HistEl, el);
	*el = (struct dirhel){dir, null};
	return el;
}

static HistEl dirhel_captcur(void){
	Buffer wd = buffer_new(PATH_MAX*2);
	if(!wd) return null;
	if(!getcwd(wd->data, PATH_MAX*2)) retclean(null, { buffer_destroy(wd); });
	HistEl e = dirhel_new(wd->data);
	if(!e) retclean(null, { buffer_destroy(wd); });
	buffer_destr(wd);
	return e;
}

static HistEl dirhel_destroy(HistEl e){
	if(!e) return null;
	HistEl prev = e->prev;
	free(e->dir);
	free(e);
	return prev;
}

struct workdirstack {
	HistEl history;
};

WorkDirStack wdstack_new(void){
	new(WorkDirStack, s);
	*s = (struct workdirstack){dirhel_captcur()};
	if(!s->history){
		free(s);
		return null;
	}
	return s;
}

WorkDirStack wdstack_clone(WorkDirStack s){
	if(!s) return null;
	new(WorkDirStack, ss);
	ss->history = null;
	HistEl* lh = &ss->history;
	for(HistEl e = s->history; e; e = e->prev){
		string_mut restr = strdup(e->dir);
		if(!restr) retclean(null, { wdstack_destroy(ss); });
		HistEl lhp = dirhel_new(restr);
		if(!lhp) retclean(null, { free(restr); wdstack_destroy(ss); });
		*lh = lhp;
	}
	return ss;
}

void wdstack_destroy(WorkDirStack s){
	if(!s) return;
	for(HistEl h = s->history; h; h = dirhel_destroy(h));
	free(s);
}

string wdstack_get(WorkDirStack s, size_t depth){
	if(!s) return null;
	HistEl h = s->history;
	for(; h && depth > 0; h = h->prev, depth--);
	return h ? h->dir : null;
}

#ifdef _WIN32
#define isabs(path) path[0] && path[1] && path[1] == ':'
#else
#define isabs(path) path[0] == '/'
#endif

Result wdstack_changedir(WorkDirStack s, string dir){
	if(!s || !dir) return Error;
	if(chdir(dir) < 0) return Error;
	HistEl he = dirhel_captcur();
	if(!he) return Error;
	he->prev = s->history;
	s->history = he;
	return Ok;
}

Result wdstack_reapply(WorkDirStack s){
	return !s || !s->history ? Error : chdir(s->history->dir) > 0 ? Error : Ok;
}

Result wdstack_goback(WorkDirStack s){
	if(!s) return Error;
	if(!s->history) return Ok;
	HistEl to = s->history->prev;
	if(to && chdir(to->dir) < 0) return Error;
	s->history = dirhel_destroy(s->history);
	return Ok;
}
