// #include <parc24/ccmdstore.h>
typedef void (*CCMD)(void);

#define _TRIE_IMPL_T CCMD
#include <util/trie.h>

typedef Trie CCMDStore;

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

static void ccmd_destroy(ATTR_UNUSED CCMD ccmd){}
static CCMD ccmd_clone(CCMD ccmd){ return ccmd; }

CCMDStore ccmdstore_new(void){
	return trie_new(ccmd_destroy);
}

void ccmdstore_destroy(CCMDStore s){
	trie_destroy(s);
}

CCMDStore ccmdstore_clone(CCMDStore store){
	return trie_clone(store, ccmd_clone);
}

CCMD ccmdstore_get(CCMDStore s, string cmd){
	return trie_get(s, cmd);
}

Result ccmdstore_set(CCMDStore store, string cmd, CCMD exe){
	if(!exe) return Error;
	return trie_add(store, cmd, exe);
}
