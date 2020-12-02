#define _TRIE_IMPL_T string_mut
#include <util/trie.h>

typedef Trie AliasStore;

#define _ALIASSTORE_IMPL
#include <parc24/aliastore.h>

#include <util/null.h>
#include <util/string.h>
#include <stdlib.h>

static void string_mut_destroy(string_mut str){
	free(str);
}

static string_mut string_mut_clone(string_mut str){
	return str ? strdup(str) : null;
}

AliasStore aliastore_new(void){
	return trie_new(string_mut_destroy);
}

void aliastore_destroy(AliasStore t){
	return trie_destroy(t);
}

AliasStore aliastore_clone(AliasStore store){
	return trie_clone(store, string_mut_clone);
}

string_mut aliastore_get(AliasStore t, string a){
	return trie_get(t, a);
}

Result aliastore_add(AliasStore t, string a, string val){
	if(!val) return Error;
	string_mut vdup = strdup(val);
	if(!vdup) return Error;
	Result addr = trie_add(t, a, vdup);
	if(!IsOk(addr)) free(vdup);
	return addr;
}

Result aliastore_remove(AliasStore store, string a){
	return trie_remove(store, a);
}
