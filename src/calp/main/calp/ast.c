#include <calp/ast.h>

#include <stdlib.h>
#include <calp/util/null.h>
#include <calp/grammar/internals.h>

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
