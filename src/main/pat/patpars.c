#include "pati.h"

#define _PATTERN_DEF
typedef Automaton Pattern;

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
DEF_SYMBOL_TERMINAL(achar, {
	return *str ? str+1 : null;
})

//[::]

#define DEF_SYMBOL_TERMINAL_CLASS(clasname) DEF_SYMBOL_TERMINAL(clas_##clasname, { return str && strpref("[:"#clasname":]", str) ? str + (sizeof("[:"#clasname":]")/sizeof(char)) : null; })
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

DEF_GROUP(acharoesc,
	RULE(SYMBOL_T(bkslsh); SYMBOL_T(achar));
	RULE(SYMBOL_T(achar))
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
	RULE(SYMBOL_G(acharoesc))
)

DEF_SYMBOL_TERMINAL(s0, { return str ? str : null; });
#define DEF_GKLEENE(name, r0symbols) DEF_GROUP(name, RULE(r0symbols; SYMBOL_G(name)); RULE(SYMBOL_T(s0)))
#define DEF_GOPT(name, rules) DEF_GROUP(name, rules; RULE(SYMBOL_T(s0)))

DEF_GOPT(clr_neg, RULE(SYMBOL_T(excl)); RULE(SYMBOL_T(caret)))

DEF_GKLEENE(clr_els, SYMBOL_G(clr_el);)
DEF_GROUP(clre, RULE(SYMBOL_T(brakl); SYMBOL_G(clr_neg); SYMBOL_G(clr_el); SYMBOL_G(clr_els); SYMBOL_T(brakr)))


// //

DEF_GROUP(pat_el,
	RULE(SYMBOL_G(clre));
	RULE(SYMBOL_T(qmrk));
	RULE(SYMBOL_T(star));
	RULE(SYMBOL_G(acharoesc))
)

DEF_GKLEENE(pat_els, SYMBOL_G(pat_el))

DEF_SYMBOL_TERMINAL(eoi, { return str && !*str ? str : null; })
DEF_GROUP(entry, RULE(SYMBOL_G(pat_els); SYMBOL_T(eoi)))

DEF_GRAMMAR(pattern_simple,
	GROUP(acharoesc);
	GROUP(clr_el);
	GROUP(clr_neg);
	GROUP(clr_els);
	GROUP(clre);
	GROUP(pat_el);
	GROUP(pat_els);
	GROUP(entry)
)

#include <calp/parser/build.h>

Parser patpars_new(void){
	Grammar g = pattern_simple();
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

PatCompResult pattern_parse(Parser parser, string str, bool doublestar){
	if(!parser || !str) return Error_T(patcomp_result, {"invalid input"});
	ParseResult past = parser_parse(parser, lexer0, str, entry);
	if(!IsOk_T(past)) return Error_T(patcomp_result, past.r.error);
	return patravast(past.r.ok.ast, doublestar);
}

#include <ctype.h>

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
		if(sid == achar){
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
		// if(sid == clas_word) return posixchaclamkpat(isword); TODO
		if(sid == clas_xdigit) return posixchaclamkpat(isxdigit);
		return Error_T(patcomp_result, {"AST (leaf) not recognized"});
	}
	const GroupId gid = ast->d.group.groupId;
	if(gid == acharoesc) return patravast(ast->d.group.children[ast->d.group.cc-1], doublestar);
	if(gid == clr_el){
		switch(ast->d.group.cc){
			case 3: {
				unsigned char crfrom = ast->d.group.children[0]->d.leaf.val[0];
				unsigned char crto = ast->d.group.children[0]->d.leaf.val[1];
				if(crto < crfrom) return Ok_T(patcomp_result, null);
				State s1 = patstate_new(false);
				State s2 = patstate_new(true);
				if(!s1 || !s2) retclean(Error_T(patcomp_result, {"failed to create states"}), { patstate_destroy(s1); patstate_destroy(s2); });
				for(unsigned char c = crfrom; c < crto; c++) if(!IsOk(patstate_tradd(s1, patransition_new(c, s2)))) retclean(Error_T(patcomp_result, {"failed to add transition"}), { patstate_destroy(s1); patstate_destroy(s2); });
				return Ok_T(patcomp_result, s1);
			}
			case 1: return patravast(ast->d.group.children[0], doublestar);
			default: break;
		}
	}
	if(gid == clr_els){
		bool negate = ast->d.group.children[1]->d.group.children[0]->d.leaf.symbolId != s0;
		PatCompResult ce0 = patravast(ast->d.group.children[2], doublestar);
		if(!IsOk_T(ce0)) return ce0;
		Automaton a = ce0.r.ok;
		for(AST ne = ast->d.group.children[3]; ne->d.group.cc > 1; ne = ne->d.group.children[2]){
			PatCompResult nep = patravast(ne, doublestar);
			if(!IsOk_T(nep)) retclean(nep, { auto_destroy(a); });
			Automaton na = auto_merge(a, nep.r.ok, true);
			auto_destroy(a);
			if(!na) return Error_T(patcomp_result, {"failed to unite class elements"});
			a = na;
		}
		if(negate && !auto_negate(a)) retclean(Error_T(patcomp_result, {"failed to negate class"}), { auto_destroy(a); });
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
		if(!a) retclean(Error_T(patcomp_result, {"failed to concatenate elements"}), { auto_destroy(a2r.r.ok); auto_destroy(a1r.r.ok); });
		return Ok_T(patcomp_result, a);
	}
	return Error_T(patcomp_result, {"AST (group) not recognized"});
}
