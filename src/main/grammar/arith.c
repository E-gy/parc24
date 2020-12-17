#include <calp/grammar/define.h>

#include <calp/parser/build.h>
#include <calp/parser/fun.h>
#include <calp/lexers.h>
#include <util/null.h>
#include <util/str2i.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef _ARITH_DEF
#define _ARITH_DEF
typedef Parser Arithmetics;
#endif

#include <parc24/arith.h>

DEF_SYMBOL_TERMINAL(eof, { return !*str ? str : null; })
// DEF_SYMBOL_TERMINAL(s0, { return str ? str : null; })
DEF_SYMBOL_TERMINAL(number, {
	if(!str || !isdigit(*str)) return null;
	/*if(!isdigit(*str)){
		if(*str != '-') return NULL;
		str++;
		if(!isdigit(*str)) return NULL;
	}*/
	for(; isdigit(*str); str++);
	return str;
})
#define DEF_SYMBOL_TERMINAL_1xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr ? str+1 : null; })
#define DEF_SYMBOL_TERMINAL_2xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr && str[1] == chr ? str+2 : null; })
DEF_SYMBOL_TERMINAL_1xCHAR(minus, '-')
DEF_SYMBOL_TERMINAL_1xCHAR(plus, '+')
DEF_SYMBOL_TERMINAL_1xCHAR(star, '*')
DEF_SYMBOL_TERMINAL_1xCHAR(slash, '/')
DEF_SYMBOL_TERMINAL_1xCHAR(percent, '%')
DEF_SYMBOL_TERMINAL_1xCHAR(amp, '&')
DEF_SYMBOL_TERMINAL_1xCHAR(vpip, '|')
DEF_SYMBOL_TERMINAL_1xCHAR(caret, '^')
DEF_SYMBOL_TERMINAL_1xCHAR(excl, '!')
DEF_SYMBOL_TERMINAL_1xCHAR(tilde, '~')
DEF_SYMBOL_TERMINAL_2xCHAR(starstar, '*')
DEF_SYMBOL_TERMINAL_2xCHAR(ampamp, '&')
DEF_SYMBOL_TERMINAL_2xCHAR(vpipvpip, '|')
DEF_SYMBOL_TERMINAL_1xCHAR(parl, '(')
DEF_SYMBOL_TERMINAL_1xCHAR(parr, ')')
static Group ngn();
static Group l0();
static Group l1();
DEF_GROUP(pOm, RULE(SYMBOL_T(plus)); RULE(SYMBOL_T(minus)))
DEF_GROUP(tOd, RULE(SYMBOL_T(star)); RULE(SYMBOL_T(slash)); RULE(SYMBOL_T(percent)))
DEF_GROUP(l0,
	RULE(SYMBOL_G(ngn); SYMBOL_T(starstar); SYMBOL_G(l0));
	RULE(SYMBOL_G(ngn))
)
DEF_GROUP(l1,
	RULE(SYMBOL_G(l1); SYMBOL_G(tOd); SYMBOL_G(l0));
	RULE(SYMBOL_G(l0))
)
DEF_GROUP(l2,
	RULE(SYMBOL_G(l2); SYMBOL_G(pOm); SYMBOL_G(l1));
	RULE(SYMBOL_G(l1))
)
DEF_GROUP(l3,
	RULE(SYMBOL_G(l3); SYMBOL_T(amp); SYMBOL_G(l3));
	RULE(SYMBOL_G(l2))
)
DEF_GROUP(l4,
	RULE(SYMBOL_G(l4); SYMBOL_T(caret); SYMBOL_G(l3));
	RULE(SYMBOL_G(l3))
)
DEF_GROUP(l5,
	RULE(SYMBOL_G(l5); SYMBOL_T(vpip); SYMBOL_G(l4));
	RULE(SYMBOL_G(l4))
)
DEF_GROUP(l6,
	RULE(SYMBOL_G(l6); SYMBOL_T(ampamp); SYMBOL_G(l5));
	RULE(SYMBOL_G(l5))
)
DEF_GROUP(l7,
	RULE(SYMBOL_G(l7); SYMBOL_T(vpipvpip); SYMBOL_G(l6));
	RULE(SYMBOL_G(l6))
)
DEF_GROUP(ng,
	RULE(SYMBOL_T(parl); SYMBOL_G(l7); SYMBOL_T(parr));
	RULE(SYMBOL_T(number))
)
DEF_GROUP(ngn,
	RULE(SYMBOL_T(minus); SYMBOL_G(ngn));
	RULE(SYMBOL_T(excl); SYMBOL_G(ngn));
	RULE(SYMBOL_T(tilde); SYMBOL_G(ngn));
	RULE(SYMBOL_G(ng))
)
DEF_GROUP(entry, RULE(SYMBOL_G(l7); SYMBOL_T(eof)))
DEF_GRAMMAR(math,
	GROUP(ng);
	GROUP(ngn);
	GROUP(pOm);
	GROUP(tOd);
	GROUP(l0);
	GROUP(l1);
	GROUP(l2);
	GROUP(l3);
	GROUP(l4);
	GROUP(l5);
	GROUP(l6);
	GROUP(l7);
	GROUP(entry)
)

static ArithResult arith_travast(AST ast){
	if(!ast) return Error_T(arith_result, {"null ast"});
	if(ast->type == AST_LEAF){
		const TerminalSymbolId sid = ast->d.leaf.symbolId;
		if(sid == number){
			Str2IResult num = str2i(ast->d.leaf.val);
			return IsOk_T(num) ? Ok_T(arith_result, num.r.ok) : Error_T(arith_result, {"not a valid number"});
		}
		return Error_T(arith_result, {"AST (leaf) not recognized"});
	}
	const GroupId gid = ast->d.group.groupId;
	if(gid == l0){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		if(trr.r.ok < 0) return Error_T(arith_result, {"exponent less than 0"});
		arithnum pow = 1;
		for(arithnum i = 0; i < trr.r.ok; i++) pow *= trl.r.ok;
		return Ok_T(arith_result, pow);
	}
	if(gid == l1){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		TerminalSymbolId op = ast->d.group.children[1]->d.group.children[0]->d.leaf.symbolId;
		if(op == slash && trr.r.ok == 0) return Error_T(arith_result, {"division by 0"});
		if(op == percent && trr.r.ok == 0) return Error_T(arith_result, {"modulo by 0"});
		return Ok_T(arith_result, op == star ? trl.r.ok * trr.r.ok : op == slash ? trl.r.ok / trr.r.ok : trl.r.ok % trr.r.ok);
	}
	if(gid == l2){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		TerminalSymbolId op = ast->d.group.children[1]->d.group.children[0]->d.leaf.symbolId;
		return Ok_T(arith_result, op == plus ? trl.r.ok + trr.r.ok : trl.r.ok - trr.r.ok);
	}
	if(gid == l3){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		return Ok_T(arith_result, trl.r.ok & trr.r.ok);
	}
	if(gid == l4){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		return Ok_T(arith_result, trl.r.ok ^ trr.r.ok);
	}
	if(gid == l5){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		return Ok_T(arith_result, trl.r.ok | trr.r.ok);
	}
	if(gid == l6){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		if(trl.r.ok == 0) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		return Ok_T(arith_result, trl.r.ok && trr.r.ok);
	}
	if(gid == l7){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trl = arith_travast(ast->d.group.children[0]);
		if(!IsOk_T(trl)) return trl;
		if(trl.r.ok != 0) return trl;
		ArithResult trr = arith_travast(ast->d.group.children[2]);
		if(!IsOk_T(trr)) return trr;
		return Ok_T(arith_result, trl.r.ok || trr.r.ok);
	}
	if(gid == ng) return arith_travast(ast->d.group.children[ast->d.group.cc > 1 ? 1 : 0]);
	if(gid == ngn){
		if(ast->d.group.cc == 1) return arith_travast(ast->d.group.children[0]);
		ArithResult trn = arith_travast(ast->d.group.children[1]);
		if(!IsOk_T(trn)) return trn;
		TerminalSymbolId op = ast->d.group.children[0]->d.leaf.symbolId;
		return Ok_T(arith_result, op == minus ? -trn.r.ok : op == excl ? !trn.r.ok : op == tilde ? ~trn.r.ok : trn.r.ok);
	}
	if(gid == entry) return arith_travast(ast->d.group.children[0]);
	return Error_T(arith_result, {"AST (group) not recognized"});
}

Arithmetics arith_new(void){
	Grammar gr = math();
	if(!gr) return null;
	ParserBuildResult pars = parser_build(gr);
	if(!IsOk_T(pars)){
		grammar_destroy(gr);
		return null;
	}
	return pars.r.ok;
}

void arith_destroy(Arithmetics arith){
	parser_destroy(arith);
}

ArithResult arith_eval(Arithmetics arith, string expr){
	ParseResult ast = parser_parse(arith, lexer_spacebegone, expr, entry);
	if(!IsOk_T(ast)) return Error_T(arith_result, ast.r.error);
	ArithResult res = arith_travast(ast.r.ok.ast);
	ast_destroy(ast.r.ok.ast);
	return res;
}
