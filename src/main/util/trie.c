#include "trie.h"

#include <stdlib.h>
#include "null.h"
#include "string.h"

struct trie {
	Trie_V_Destructor _d;
	char c;
	Trie_V val;
	Trie sibling;
	Trie children;
};

static Trie _trie_new(Trie_V_Destructor _d, char c, string_mut val, Trie sibling, Trie children){
	new(Trie, t);
	*t = (struct trie){_d, c, val, sibling, children};
	return t;
}

#define _trie_new_c(_d, c) _trie_new(_d, c, null, null, null)
#define _trie_new_val(_d, c, val) _trie_new(_d, c, val, null, null)

static void _trie_destroy(Trie t){
	if(!t) return;
	t->_d(t->val);
	free(t);
}

static Trie _trie_add(Trie t, string s, string_mut val){
	if(!t || !s || !*s) return null;
	if(!t->children){
		t->children = _trie_new_c(t->_d, *s);
		if(s[1]) return _trie_add(t->children, s + 1, val);
		t->children->val = val;
		return t->children;
	}
	Trie *ins = &t->children;
	for(; *ins && (*ins)->c < *s; ins = &((*ins)->sibling));
	if(*ins && (*ins)->c == *s) {
		if(s[1]) return _trie_add(*ins, s + 1, val);
		if((*ins)->val) t->_d((*ins)->val);
		(*ins)->val = val;
		return *ins;
	}
	*ins = _trie_new(t->_d, *s, null, *ins, null);
	if(s[1]) return _trie_add(*ins, s + 1, val);
	(*ins)->val = val;
	return *ins;
}

struct rem_res {
	bool removed;
	Trie newchild;
};
#define rem_res_c(r, nc) (struct rem_res){r, nc}

static struct rem_res _trie_remove(Trie t, string s){
	if(!t || !s) return rem_res_c(false, t);
	if(!*s){
		if(!t->c || !t->val) return rem_res_c(false, t);
		if(!t->children) {
			Trie repl = t->sibling;
			_trie_destroy(t);
			return rem_res_c(true, repl);
		}
		t->_d(t->val);
		t->val = null;
		return rem_res_c(true, t);
	}
	Trie *rc = &t->children;
	for(; *rc && (*rc)->c < *s; rc = &((*rc)->sibling));
	if(!*rc || (*rc)->c != *s) return rem_res_c(false, t);
	struct rem_res rrr = _trie_remove(*rc, s + 1);
	*rc = rrr.newchild;
	if(!t->children && t->c && !t->val){
		Trie repl = t->sibling;
		_trie_destroy(t);
		return rem_res_c(rrr.removed, repl);
	}
	return rem_res_c(rrr.removed, t);
}

static Trie _trie_find(Trie t, string s){
	if(!t || !s) return null;
	if(!*s)return t;
	for(Trie tt = t->children; tt && tt->c <= *s; tt = tt->sibling) if(tt->c == *s) return _trie_find(tt, s + 1);
	return null;
}

//Impl

Trie trie_new(Trie_V_Destructor d){
	return _trie_new_c(d, '\0');
}

void trie_destroy(Trie t){
	if(!t) return;
	trie_destroy(t->children);
	trie_destroy(t->sibling);
	_trie_destroy(t);
}

Trie trie_clone(Trie store, Trie_V_Copy cloner){
	if(!store) return null;
	Trie clone = _trie_new(store->_d, store->c, cloner(store->val), null, null);
	if(!clone) return null;
	if((store->children && !(clone->children = trie_clone(store->children, cloner))) || (store->sibling && !(clone->sibling = trie_clone(store->sibling, cloner)))){
		trie_destroy(clone);
		return null;
	}
	return clone;
}

Trie_V trie_get(Trie t, string key){
	Trie r = _trie_find(t, key);
	return r ? r->val : null;
}

Result trie_add(Trie t, string key, Trie_V v){
	return _trie_add(t, key, v) ? Ok : Error;
}

Result trie_remove(Trie store, string key){
	return _trie_remove(store, key).removed ? Ok : Error;
}
