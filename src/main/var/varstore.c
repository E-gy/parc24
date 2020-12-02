#define _TRIE_IMPL_T string_mut
#include <util/trie.h>

typedef Trie VarStore;

#define _VARSTORE_IMPL
#include <parc24/var_store.h>

#include <util/null.h>
#include <util/string.h>
#include <stdlib.h>

static void string_mut_destroy(string_mut str){
	free(str);
}

static string_mut string_mut_clone(string_mut str){
	return str ? strdup(str) : null;
}

VarStore varstore_new(void){
	return trie_new(string_mut_destroy);
}

void varstore_destroy(VarStore t){
	return trie_destroy(t);
}

VarStore varstore_clone(VarStore store){
	return trie_clone(store, string_mut_clone);
}

string_mut varstore_get(VarStore t, string var){
	return trie_get(t, var);
}

Result varstore_add(VarStore t, string var, string val){
	if(!val) return Error;
	string_mut vdup = strdup(val);
	if(!vdup) return Error;
	Result addr = trie_add(t, var, vdup);
	if(!IsOk(addr)) free(vdup);
	return addr;
}

Result varstore_remove(VarStore store, string var){
	return trie_remove(store, var);
}