#include <parc24/ccmdstore.h>

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

//TODO we literally have a Trie used elsewhere ._.
//we can certainly do better than a flippin linked list..? 

struct ccmdstoreel;
typedef struct ccmdstoreel* CCMDSEl;
struct ccmdstoreel {
	string_mut cmd;
	CCMD exe;
	CCMDSEl next;
};

static CCMDSEl ccmdstoreel_new(string_mut cmd, CCMD exe){
	if(!cmd || !exe) return null;
	new(CCMDSEl, e);
	*e = (struct ccmdstoreel){cmd, exe, null};
	return e;
}

struct ccmdstore {
	CCMDSEl els;
};

CCMDStore ccmdstore_new(){
	new(CCMDStore, s);
	*s = (struct ccmdstore){null};
	return s;
}

void ccmdstore_destroy(CCMDStore s){
	if(!s) return;
	for(CCMDSEl e = s->els; e;){
		CCMDSEl next = e->next;
		free(e->cmd);
		free(e);
		e = next;
	}
	free(s);
}

CCMD ccmdstore_get(CCMDStore s, string cmd){
	if(!s || !cmd) return null;
	for(CCMDSEl e = s->els; e; e = e->next){
		int cmp = strcmp(e->cmd, cmd);
		if(cmp == 0) return e->exe;
		if(cmp > 0) return null;
	}
	return null;
}

Result ccmdstore_set(CCMDStore s, string cmd, CCMD exe){
	if(!s || !cmd || !exe) return Error;
	string_mut cmdv = strdup(cmd);
	if(!cmdv) return Error;
	CCMDSEl* e = &s->els;
	for(; *e && strcmp((*e)->cmd, cmd) < 0; e = &((*e)->next));
	if(e && streq((*e)->cmd, cmd)){
		CCMDSEl el = *e;
		free(el->cmd);
		el->cmd = cmdv;
		el->exe = exe;
	} else {
		CCMDSEl el = ccmdstoreel_new(cmdv, exe);
		if(!el){
			free(cmdv);
			return Error;
		}
		el->next = *e;
		*e = el;
	}
	return Ok;
}

