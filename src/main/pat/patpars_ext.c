#include "patpars.h"

#include <calp/grammar/define.h>
#include <util/null.h>
#include <util/string.h>

#define DEF_SYMBOL_TERMINAL_1xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr ? str+1 : null; })
DEF_SYMBOL_TERMINAL_1xCHAR(qmrk, '?')
DEF_SYMBOL_TERMINAL_1xCHAR(star, '*')
DEF_SYMBOL_TERMINAL_1xCHAR(excl, '!')
DEF_SYMBOL_TERMINAL_1xCHAR(caret, '^')
DEF_SYMBOL_TERMINAL_1xCHAR(dash, '-')
DEF_SYMBOL_TERMINAL_1xCHAR(bkslsh, '\\')
DEF_SYMBOL_TERMINAL_1xCHAR(brakl,'[')
DEF_SYMBOL_TERMINAL_1xCHAR(brakr,']')
DEF_SYMBOL_TERMINAL_1xCHAR(parl,'(')
DEF_SYMBOL_TERMINAL_1xCHAR(parr,')')
DEF_SYMBOL_TERMINAL_1xCHAR(vpipe,'|')
DEF_SYMBOL_TERMINAL_1xCHAR(plus,'+')
DEF_SYMBOL_TERMINAL_1xCHAR(atdog,'@')
DEF_SYMBOL_TERMINAL(achar, {
	return *str ? str+1 : null;
})
DEF_SYMBOL_TERMINAL(achar_clr, {
	return *str && *str != '[' && *str != ']' ? str+1 : null;
})
DEF_SYMBOL_TERMINAL(achar_ext, {
	return *str && *str != '|' && *str != '?' && *str != '*' && *str != '+' && *str != '@' && *str != '!' && *str != '(' && *str != ')' && *str != '[' && *str != ']' ? str+1 : null;
})

//[::]

#define DEF_SYMBOL_TERMINAL_CLASS(clasname) DEF_SYMBOL_TERMINAL(clas_##clasname, { return str && strpref("[:"#clasname":]", str) ? str + (sizeof("[:"#clasname":]")/sizeof(char) - 1) : null; })
DEF_SYMBOL_TERMINAL_CLASS(alnum)
DEF_SYMBOL_TERMINAL_CLASS(alpha)
DEF_SYMBOL_TERMINAL_CLASS(ascii)
DEF_SYMBOL_TERMINAL_CLASS(blank)
DEF_SYMBOL_TERMINAL_CLASS(cntrl)
DEF_SYMBOL_TERMINAL_CLASS(digit)
DEF_SYMBOL_TERMINAL_CLASS(graph)
DEF_SYMBOL_TERMINAL_CLASS(lower)
DEF_SYMBOL_TERMINAL_CLASS(print)
DEF_SYMBOL_TERMINAL_CLASS(punct)
DEF_SYMBOL_TERMINAL_CLASS(space)
DEF_SYMBOL_TERMINAL_CLASS(upper)
DEF_SYMBOL_TERMINAL_CLASS(word)
DEF_SYMBOL_TERMINAL_CLASS(xdigit)

// escape

DEF_GROUP(achar_clr_oesc,
	RULE(SYMBOL_T(bkslsh); SYMBOL_T(achar));
	RULEp(-100, SYMBOL_T(achar_clr))
)
DEF_GROUP(achar_ext_oesc,
	RULE(SYMBOL_T(bkslsh); SYMBOL_T(achar));
	RULEp(-100, SYMBOL_T(achar_ext))
)

// []

DEF_GROUP(clr_el,
	RULE(SYMBOL_T(achar); SYMBOL_T(dash); SYMBOL_T(achar));
	RULE(SYMBOL_T(clas_alnum));
	RULE(SYMBOL_T(clas_alpha));
	RULE(SYMBOL_T(clas_ascii));
	RULE(SYMBOL_T(clas_blank));
	RULE(SYMBOL_T(clas_cntrl));
	RULE(SYMBOL_T(clas_digit));
	RULE(SYMBOL_T(clas_graph));
	RULE(SYMBOL_T(clas_lower));
	RULE(SYMBOL_T(clas_print));
	RULE(SYMBOL_T(clas_punct));
	RULE(SYMBOL_T(clas_space));
	RULE(SYMBOL_T(clas_upper));
	RULE(SYMBOL_T(clas_word));
	RULE(SYMBOL_T(clas_xdigit));
	RULE(SYMBOL_G(achar_clr_oesc))
)

DEF_SYMBOL_TERMINAL(s0, { return str ? str : null; })
#define DEF_GKLEENE(name, r0symbols) DEF_GROUP(name, RULE(r0symbols; SYMBOL_G(name)); RULE(SYMBOL_T(s0)))
#define DEF_GOPT(name, rules) DEF_GROUP(name, rules; RULE(SYMBOL_T(s0)))

DEF_GOPT(clr_neg, RULE(SYMBOL_T(excl)); RULE(SYMBOL_T(caret)))

DEF_GKLEENE(clr_els, SYMBOL_G(clr_el))
DEF_GROUP(clre, RULE(SYMBOL_T(brakl); SYMBOL_G(clr_neg); SYMBOL_G(clr_el); SYMBOL_G(clr_els); SYMBOL_T(brakr)))

// (|)

static Group pat_els();

DEF_GROUP(extg_el, RULE(SYMBOL_G(pat_els)))

DEF_GKLEENE(extg_els, SYMBOL_T(vpipe); SYMBOL_G(extg_el))
DEF_GROUP(extg_op, 
	RULE(SYMBOL_T(qmrk));
	RULE(SYMBOL_T(star));
	RULE(SYMBOL_T(excl));
	RULE(SYMBOL_T(plus));
	RULE(SYMBOL_T(atdog))
)
DEF_GROUPp(extg, 10, RULE(SYMBOL_G(extg_op); SYMBOL_T(parl); SYMBOL_G(extg_el); SYMBOL_G(extg_els); SYMBOL_T(parr)))

// //

DEF_GROUP(pat_el,
	RULE(SYMBOL_G(extg));
	RULE(SYMBOL_G(clre));
	RULE(SYMBOL_T(qmrk));
	RULE(SYMBOL_T(star));
	RULE(SYMBOL_G(achar_ext_oesc))
)

DEF_GKLEENE(pat_els, SYMBOL_G(pat_el))

DEF_SYMBOL_TERMINAL(eoi, { return str && !*str ? str : null; })
DEF_GROUP(entry, RULE(SYMBOL_G(pat_els); SYMBOL_T(eoi)))

DEF_GRAMMAR(pattern_ext,
	GROUP(achar_clr_oesc);
	GROUP(achar_ext_oesc);
	GROUP(clr_el);
	GROUP(clr_neg);
	GROUP(clr_els);
	GROUP(clre);
	GROUP(extg_el);
	GROUP(extg_els);
	GROUP(extg_op);
	GROUP(extg);
	GROUP(pat_el);
	GROUP(pat_els);
	GROUP(entry)
)

#include <calp/parser/build.h>

Parser patpars_ext_new(void){
	Grammar g = pattern_ext();
	if(!g) return null;
	ParserBuildResult p = parser_build(g);
	if(!IsOk_T(p)){
		grammar_destroy(g);
		return null;
	}
	return p.r.ok;
}

#include <calp/ast.h>

static PatCompResult patravast(AST ast, bool doublestar);

#include <calp/parser/fun.h>
#include <calp/lexers.h>

PatCompResult pattern_parse_ext(Parser parser, string str, bool doublestar){
	if(!parser || !str) return Error_T(patcomp_result, {"invalid input"});
	ParseResult past = parser_parse(parser, lexer0, str, entry);
	if(!IsOk_T(past)) return Error_T(patcomp_result, past.r.error);
	PatCompResult pat = patravast(past.r.ok.ast, doublestar);
	ast_destroy(past.r.ok.ast);
	return pat;
}

#include <ctype.h>
int char_isword(int c);

static PatCompResult posixchaclamkpat(int (*predicate)(int)){
	if(!predicate) return Error_T(patcomp_result, {"invalid char class predicate"});
	State s1 = patstate_new(false);
	State s2 = patstate_new(true);
	if(!s1 || !s2) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s1); patstate_destroy(s2); });
	for(unsigned char c = 0; c < 128; c++) if(predicate(c)) if(!IsOk(patstate_tradd(s1, patransition_new(c, s2)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s1); patstate_destroy(s2); });
	return Ok_T(patcomp_result, s1);
}

#define patast_els_isstar(ast) ast->d.group.cc > 0 && ast->d.group.children[0]->type == AST_LEAF && ast->d.group.children[0]->d.leaf.symbolId == star

static PatCompResult patravast(AST ast, bool doublestar){
	if(!ast) return Error_T(patcomp_result, {"null ast"});
	if(ast->type == AST_LEAF){
		const TerminalSymbolId sid = ast->d.leaf.symbolId;
		if(sid == star){
			State s = patstate_new(true);
			State ss = patstate_new(false);
			if(!s || !ss) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s); patstate_destroy(ss); });
			if(!IsOk(patstate_tradd(s, patransition_new('/', ss)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s); patstate_destroy(ss); });
			s->defolt = s;
			return Ok_T(patcomp_result, s);
		}
		if(sid == qmrk){
			State s1 = patstate_new(false);
			State s2 = patstate_new(true);
			State ss = patstate_new(false);
			if(!s1 || !s2 || !ss) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s1); patstate_destroy(s2); patstate_destroy(ss); });
			if(!IsOk(patstate_tradd(s1, patransition_new('/', ss)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s1); patstate_destroy(s2); patstate_destroy(ss); });
			s1->defolt = s2;
			return Ok_T(patcomp_result, s1);
		}
		if(sid == achar || sid == achar_clr || sid == achar_ext){
			State s1 = patstate_new(false);
			State s2 = patstate_new(true);
			if(!s1 || !s2) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s1); patstate_destroy(s2); });
			if(!IsOk(patstate_tradd(s1, patransition_new(ast->d.leaf.val[0], s2)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s1); patstate_destroy(s2); });
			return Ok_T(patcomp_result, s1);
		}
		if(sid == clas_alnum) return posixchaclamkpat(isalnum);
		if(sid == clas_alpha) return posixchaclamkpat(isalpha);
		if(sid == clas_ascii) return posixchaclamkpat(isascii);
		if(sid == clas_blank) return posixchaclamkpat(isblank);
		if(sid == clas_cntrl) return posixchaclamkpat(iscntrl);
		if(sid == clas_digit) return posixchaclamkpat(isdigit);
		if(sid == clas_graph) return posixchaclamkpat(isgraph);
		if(sid == clas_lower) return posixchaclamkpat(islower);
		if(sid == clas_print) return posixchaclamkpat(isprint);
		if(sid == clas_punct) return posixchaclamkpat(ispunct);
		if(sid == clas_space) return posixchaclamkpat(isspace);
		if(sid == clas_upper) return posixchaclamkpat(isupper);
		if(sid == clas_word) return posixchaclamkpat(char_isword);
		if(sid == clas_xdigit) return posixchaclamkpat(isxdigit);
		return Error_T(patcomp_result, {"AST (leaf) not recognized"});
	}
	const GroupId gid = ast->d.group.groupId;
	if(gid == achar_clr_oesc || gid == achar_ext_oesc) return patravast(ast->d.group.children[ast->d.group.cc-1], doublestar);
	if(gid == clr_el){
		switch(ast->d.group.cc){
			case 3: {
				unsigned char crfrom = ast->d.group.children[0]->d.leaf.val[0];
				unsigned char crto = ast->d.group.children[2]->d.leaf.val[0];
				if(crto < crfrom) return Ok_T(patcomp_result, null);
				State s1 = patstate_new(false);
				State s2 = patstate_new(true);
				if(!s1 || !s2) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s1); patstate_destroy(s2); });
				for(unsigned char c = crfrom; c <= crto; c++) if(!IsOk(patstate_tradd(s1, patransition_new(c, s2)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s1); patstate_destroy(s2); });
				return Ok_T(patcomp_result, s1);
			}
			case 1: return patravast(ast->d.group.children[0], doublestar);
			default: break;
		}
	}
	if(gid == clre){
		bool negate = ast->d.group.children[1]->d.group.children[0]->d.leaf.symbolId != s0;
		PatCompResult ce0 = patravast(ast->d.group.children[2], doublestar);
		if(!IsOk_T(ce0)) return ce0;
		Automaton a = ce0.r.ok;
		for(AST ne = ast->d.group.children[3]; ne->d.group.cc > 1; ne = ne->d.group.children[1]){
			PatCompResult nep = patravast(ne->d.group.children[0], doublestar);
			if(!IsOk_T(nep)) retclean(nep, { auto_destroy(a); });
			Automaton na = auto_merge(a, nep.r.ok, true);
			auto_destroy(a);
			if(!na) return Error_T(patcomp_result, {"failed to unite class elements"});
			a = na;
		}
		if(negate && !IsOk(auto_negate(a))) retclean(Error_T(patcomp_result, {"failed to negate class"}), { auto_destroy(a); });
		return Ok_T(patcomp_result, a);
	}
	if(gid == extg_el) return patravast(ast->d.group.children[0], doublestar);
	if(gid == extg){
		const TerminalSymbolId op = ast->d.group.children[0]->d.group.children[0]->d.leaf.symbolId;
		PatCompResult ab = patravast(ast->d.group.children[2], doublestar);
		if(!IsOk_T(ab)) return ab;
		Automaton a = ab.r.ok;
		for(AST elsa = ast->d.group.children[3]; elsa->d.group.cc > 1; elsa = elsa->d.group.children[2]){
			PatCompResult ac = patravast(elsa->d.group.children[1], doublestar);
			if(!IsOk_T(ac)) retclean(ac, { auto_destroy(a); });
			Automaton aac = auto_or(a, ac.r.ok);
			auto_destroy(a);
			auto_destroy(ac.r.ok);
			if(!aac) return Error_T(patcomp_result, {"failed to unite extended elements"});
			a = aac;
		}
		if(op == qmrk){
			Automaton aa = auto_optional(a);
			auto_destroy(a);
			if(!aa) return Error_T(patcomp_result, {"failed to optionalize"});
			a = aa;
		} else if(op == star){
			Automaton aa = auto_kleene(a);
			auto_destroy(a);
			if(!aa) return Error_T(patcomp_result, {"failed to kleeneize"});
			a = aa;
		} else if(op == plus){
			Automaton aa = auto_kleeneplus(a);
			auto_destroy(a);
			if(!aa) return Error_T(patcomp_result, {"failed to kleeneplusize"});
			a = aa;
		} else if(op == excl) if(!IsOk(auto_negate(a))) retclean(Error_T(patcomp_result, {"failed to negatize"}), { auto_destroy(a); });
		return Ok_T(patcomp_result, a);
	}
	if(gid == pat_el) return patravast(ast->d.group.children[0], doublestar);
	if(gid == pat_els){
		if(ast->d.group.cc == 1){
			State s = patstate_new(true);
			return s ? Ok_T(patcomp_result, s) : Error_T(patcomp_result, {"failed to create new state"});
		}
		if(doublestar && patast_els_isstar(ast) && patast_els_isstar(ast->d.group.children[1])){
			State s = patstate_new(true);
			if(!s) return Error_T(patcomp_result, {"failed to create new state"});
			s->defolt = s;
			return Ok_T(patcomp_result, s);
		}
		PatCompResult a1r = patravast(ast->d.group.children[0], doublestar);
		if(!IsOk_T(a1r)) return a1r;
		PatCompResult a2r = patravast(ast->d.group.children[1], doublestar);
		if(!IsOk_T(a2r)) retclean(a2r, { auto_destroy(a1r.r.ok); });
		Automaton a = auto_concat(a1r.r.ok, a2r.r.ok);
		auto_destroy(a2r.r.ok);
		auto_destroy(a1r.r.ok);
		if(!a) return Error_T(patcomp_result, {"failed to concatenate elements"});
		return Ok_T(patcomp_result, a);
	}
	if(gid == entry) return patravast(ast->d.group.children[0], doublestar);
	return Error_T(patcomp_result, {"AST (group) not recognized"});
}
