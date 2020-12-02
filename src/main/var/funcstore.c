#include <ptypes.h>
#include <calp/ast.h>

#define _TRIE_IMPL_T AST
#include <util/trie.h>

typedef Trie FuncStore;

#define _FUNCSTORE_IMPL
#include <parc24/funcstore.h>

#include <util/null.h>
#include <util/string.h>
#include <stdlib.h>

FuncStore funcstore_new(void){
	return trie_new(ast_destroy);
}

void funcstore_destroy(FuncStore t){
	return trie_destroy(t);
}

FuncStore funcstore_clone(FuncStore store){
	return trie_clone(store, ast_clone);
}

AST funcstore_get(FuncStore t, string f){
	return trie_get(t, f);
}

Result funcstore_add(FuncStore t, string f, AST ast){
	if(!ast) return Error;
	AST adup = ast_clone(ast);
	if(!adup) return Error;
	Result addr = trie_add(t, f, adup);
	if(!IsOk(addr)) ast_destroy(adup);
	return addr;
}
