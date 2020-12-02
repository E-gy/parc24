#include <calp/ast.h>

#include <stdlib.h>
#include <calp/util/null.h>
#include <calp/grammar/internals.h>
#include <calp/util/string.h>

AST ast_new_group(Symbol symbol, Group g, size_t children_count){
	AST ast = calloc(sizeof(*ast)/sizeof(ast) + children_count, sizeof(*ast->d.group.children));
	if(!ast) return null;
	*ast = (struct ast){AST_GROUP, symbol, {.group = {g->id, g, children_count}}};
	return ast;
}

AST ast_new_leaf(Symbol symbol, string_mut val){
	new(AST, ast);
	*ast = (struct ast){ AST_LEAF, symbol, {.leaf = { symbol->val.term.id, val }} };
	return ast;
}

void ast_destroy(AST ast){
	if(!ast) return;
	switch (ast->type){
		case AST_LEAF:
			free(ast->d.leaf.val);
			break;
		case AST_GROUP:
			for(size_t i = 0; i < ast->d.group.cc; i++) ast_destroy(ast->d.group.children[i]);
			break;
	}
	free(ast);
}

AST ast_clone(AST ast){
	if(!ast) return null;
	switch(ast->type){
		case AST_GROUP: {
			AST a = ast_new_group(ast->symbol, ast->d.group.group, ast->d.group.cc);
			if(!a) return null;
			for(size_t i = 0; i < a->d.group.cc; i++) if(ast->d.group.children[i]){
				AST cc = ast_clone(ast->d.group.children[i]);
				if(!cc){
					ast_destroy(a);
					return null;
				}
				a->d.group.children[i] = cc;
			}
			return a;
		}
		case AST_LEAF: {
			string_mut vdup = strdup(ast->d.leaf.val);
			if(!vdup) return null;
			AST a = ast_new_leaf(ast->symbol, vdup);
			if(!a) free(vdup);
			return a;
		}
		default: return null;
	}
}

#include <calp/util/log.h>
#include <stdbool.h>
#include "grammar/internals.h"

static void ast_log_d(AST ast, size_t d, size_t laste){
	{
		for(size_t i = 0; i < d; i++) logi((laste>>(d-i))&1 ? "  " : "│ ");
		if(d > 0) logi(laste&1 ? "└─" : "├─");
	}
	if(!ast) log("[NULL]");
	else switch(ast->type){
		case AST_LEAF:
			logf("'%s'(\"%s\")", ast->symbol->val.term.name, ast->d.leaf.val);
			break;
		case AST_GROUP:{
			logf("<%s>:", ast->d.group.group->name);
			for(size_t i = 0; i < ast->d.group.cc; i++) ast_log_d(ast->d.group.children[i], d+1, (laste<<1)|(i+1 == ast->d.group.cc ? 1 : 0));
			break;
		}
	}
}

void ast_log(AST ast){
	ast_log_d(ast, 0, 1);
}
