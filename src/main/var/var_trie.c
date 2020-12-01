#include <parc24/var_store.h>

#include <stdlib.h>
#include <util/null.h>
#include <util/string.h>

struct var_store {
	char c;
	string_mut val;
	VarStore sibling;
	VarStore children;
};

static VarStore var_trie_new(char c, string_mut val, VarStore sibling, VarStore children){
	new(VarStore, t);
	*t = (struct var_store){c, val, sibling, children};
	return t;
}

#define var_trie_new_c(c) var_trie_new(c, null, null, null)
#define var_trie_new_val(c, val) var_trie_new(c, val, null, null)

static void var_trie_destroy(VarStore t){
	if(!t) return;
	free(t->val);
	free(t);
}

static VarStore var_trie_add(VarStore t, string s, string_mut val){
	if(!t || !s || !*s) return null;
	if(!t->children){
		t->children = var_trie_new_c(*s);
		if(s[1]) return var_trie_add(t->children, s + 1, val);
		t->children->val = val;
		return t->children;
	}
	VarStore *ins = &t->children;
	for(; *ins && (*ins)->c < *s; ins = &((*ins)->sibling));
	if(*ins && (*ins)->c == *s) {
		if(s[1]) return var_trie_add(*ins, s + 1, val);
		if((*ins)->val) free((*ins)->val);
		(*ins)->val = val;
		return *ins;
	}
	*ins = var_trie_new(*s, null, *ins, null);
	if(s[1]) return var_trie_add(*ins, s + 1, val);
	(*ins)->val = val;
	return *ins;
}

struct rem_res {
	bool removed;
	VarStore newchild;
};
#define rem_res_c(r, nc) (struct rem_res){r, nc}

static struct rem_res var_trie_remove(VarStore t, string s){
	if(!t || !s) return rem_res_c(false, t);
	if(!*s){
		if(!t->c || !t->val) return rem_res_c(false, t);
		if(!t->children) {
			VarStore repl = t->sibling;
			var_trie_destroy(t);
			return rem_res_c(true, repl);
		}
		free(t->val);
		t->val = null;
		return rem_res_c(true, t);
	}
	VarStore *rc = &t->children;
	for(; *rc && (*rc)->c < *s; rc = &((*rc)->sibling));
	if(!*rc || (*rc)->c != *s) return rem_res_c(false, t);
	struct rem_res rrr = var_trie_remove(*rc, s + 1);
	*rc = rrr.newchild;
	if(!t->children && t->c && !t->val){
		VarStore repl = t->sibling;
		var_trie_destroy(t);
		return rem_res_c(rrr.removed, repl);
	}
	return rem_res_c(rrr.removed, t);
}

static VarStore var_trie_find(VarStore t, string s){
	if(!t || !s) return null;
	if(!*s)return t;
	for(VarStore tt = t->children; tt && tt->c <= *s; tt = tt->sibling) if(tt->c == *s) return var_trie_find(tt, s + 1);
	return null;
}

//Impl

VarStore varstore_new(void){
	return var_trie_new_c('\0');
}

void varstore_destroy(VarStore t){
	if(!t) return;
	varstore_destroy(t->children);
	varstore_destroy(t->sibling);
	var_trie_destroy(t);
}

VarStore varstore_clone(VarStore store){
	if(!store) return null;
	string_mut valcpy = strdup(store->val);
	if(store->val && !valcpy) return null;
	VarStore clone = var_trie_new(store->c, valcpy, null, null);
	if(!clone){
		free(valcpy);
		return null;
	}
	if((store->children && !(clone->children = varstore_clone(store->children))) || (store->sibling && !(clone->sibling = varstore_clone(store->sibling)))){
		varstore_destroy(clone);
		return null;
	}
	return clone;
}

string_mut varstore_get(VarStore t, string var){
	VarStore r = var_trie_find(t, var);
	return r ? r->val : null;
}

Result varstore_add(VarStore t, string var, string val){
	string_mut valcpy = strdup(val);
	VarStore r = var_trie_add(t, var, valcpy);
	if(!r) free(valcpy);
	return r ? Ok : Error;
}

Result varstore_remove(VarStore store, string var){
	return var_trie_remove(store, var).removed ? Ok : Error;
}
