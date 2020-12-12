#include <calp/parser/fun.h>

#include "internals.h"
#include <calp/util/string.h>
#include <calp/util/log.h>

Result_T(parspreresult, AST, string_v);
#define ParsPreResult struct parspreresult

static ParsPreResult parser_makast(Parser p, Lexer l, Symbol symb, string* str);
static ParsPreResult parser_makastr(Parser p, Lexer l, string* str, Rule r, Symbol symb, EntityInfo gi);

static ParsPreResult parser_makastr(Parser p, Lexer l, string* str, Rule r, Symbol symb, EntityInfo gi){
	AST gast = ast_new_group(symb, gi->i.group.group, r->symbolsc);
	size_t i = 0;
	string sstr = *str;
	for(Symbol rs = r->symbols; rs; rs = rs->next){
		ParsPreResult rsast = parser_makast(p, l, rs, &sstr);
		if(!IsOk_T(rsast)) break;
		gast->d.group.children[i++] = rsast.r.ok;
	}
	if(i != r->symbolsc){
		ast_destroy(gast);
		return Error_T(parspreresult, {"rule match failed"});
	}
	if(gi->i.group.firsts->lr.r){
		const Symbol lrfrom = gi->i.group.firsts->lr.from;
		if(lrfrom->type == SYMBOL_TYPE_GROUP && lrfrom->val.group.id == gi->i.group.groupId){ //if false, internal error - invalid state
			const Rule lrr = gi->i.group.firsts->lr.r;
			while(true){
				string ssstr = sstr;
				AST ngast = ast_new_group(symb, gi->i.group.group, lrr->symbolsc);
				Symbol lrs = lrr->symbols;
				size_t i = 0;
				for(; lrs && lrs != lrfrom; lrs = lrs->next){
					ParsPreResult rsast = parser_makast(p, l, lrs, &ssstr); //in theory, accepts empty string
					if(!IsOk_T(rsast)) break;
					ngast->d.group.children[i++] = rsast.r.ok;
				}
				if(lrs != lrfrom){ //all symbols preceding self recursion didn't want to accept empty string
					ast_destroy(ngast);
					break;
				}
				const size_t yi = i++; //growth ast is put in only on success
				for(lrs = lrs->next; lrs; lrs = lrs->next){
					ParsPreResult rsast = parser_makast(p, l, lrs, &ssstr);
					if(!IsOk_T(rsast)) break;
					ngast->d.group.children[i++] = rsast.r.ok;
				}
				if(i != lrr->symbolsc){
					ast_destroy(ngast);
					break;
				}
				(ngast->d.group.children[yi] = gast)->symbol = lrfrom;
				gast = ngast;
				sstr = ssstr;
			}
		}
	}
	*str = sstr;
	return Ok_T(parspreresult, gast);
}

static ParsPreResult parser_makast(Parser p, Lexer l, Symbol symb, string* str){
	switch(symb->type){
		case SYMBOL_TYPE_TERM: {
			IfElse_T(l(*str, symb->val.term.id), ok, {
				AST ret = ast_new_leaf(symb, strndup(ok.start, ok.end-ok.start));
				if(!ret) return Error_T(parspreresult, {"[INTERNAL] AST leaf construction failed"});
				*str = ok.next;
				return Ok_T(parspreresult, ret);
			}, err, {
				logdebug("'%s' failed to match \"%s\": %s", symb->val.term.name, *str, err.s);
				return Error_T(parspreresult, {"Terminal symbol failed to match"});
			});
		}
		case SYMBOL_TYPE_GROUP: {
			EntityInfo gi = entimap_get(p->ents, entinf_blank_group(symb->val.group.id));
			if(!gi) return Error_T(parspreresult, {"[INTERNAL] Invalid state: group not in map"});
			for(FirstListElement fl = gi->i.group.firsts->first; fl; fl = fl->next){
				if(fl->symbol->type != SYMB_TERM) return Error_T(parspreresult, {"[INTERNAL] Invalid state: non-terminal first list element"});
				if(fl->symbol->i.term.symbolId(*str)){
					ParsPreResult res = parser_makastr(p, l, str, fl->r, symb, gi);
					if(IsOk_T(res)) return res;
				}
			}
			if(gi->i.group.firsts->fallback){
				Rule r = gi->i.group.firsts->fallback;
				ParsPreResult res = parser_makastr(p, l, str, gi->i.group.firsts->fallback, symb, gi);
				if(IsOk_T(res)) return res;
			}
			logdebug("<%s> first list exhausted, no matches", gi->i.group.group->name);
			return Error_T(parspreresult, {"First list exhausted, no matches found"});
		}
		default: return Error_T(parspreresult, {"._."}); //._.
	}
} 

#include <calp/grammar/build.h>

ParseResult parser_parse(Parser p, Lexer l, string s, GroupId g0){
	if(!p || !l || !s || !g0) return Error_T(parse_result, {"Invalid args"});
	Symbol gsymb = symbol_new_group(g0, 0);
	if(!gsymb) return Error_T(parse_result, {"Allocation failed"});
	ParsPreResult result = parser_makast(p, l, gsymb, &s);
	symbol_destroy(gsymb);
	return IsOk_T(result) ? Ok_T(parse_result, {result.r.ok, s}) : Error_T(parse_result, result.r.error);
}