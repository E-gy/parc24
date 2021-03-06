#include <calp/grammar/define.h>

#include "quotexpando.h"
#include <util/null.h>
#include <util/string.h>
#include <util/buffer.h>
#include <util/str2i.h>
#include <stdlib.h>
#include <ctype.h>

enum hijakmore_op {
	HIJAKMORE_RST = 0,
	HIJAKMORE_MRK,
	HIJAKMORE_GET
};

static bool hijakmore_ctrl(enum hijakmore_op op);

//what is a word? - whatver that deserves an entire header on its own
DEF_SYMBOL_TERMINAL(word, {
	return capture_word(str);
})
#define word0chtok(str, capt, tok) (strpref(tok, str) && capt-str == ((sizeof(tok)-1)/sizeof(*tok)))
DEF_SYMBOL_TERMINAL(word_0, {
	string capt = capture_word(str);
	if(!capt) return null;
	if(word0chtok(str, capt, "if")) return null;
	if(word0chtok(str, capt, "then")) return null;
	if(word0chtok(str, capt, "elif")) return null;
	if(word0chtok(str, capt, "else")) return null;
	if(word0chtok(str, capt, "fi")) return null;
	if(word0chtok(str, capt, "do")) return null;
	if(word0chtok(str, capt, "done")) return null;
	if(word0chtok(str, capt, "for")) return null;
	if(word0chtok(str, capt, "in")) return null;
	if(word0chtok(str, capt, "while")) return null;
	if(word0chtok(str, capt, "until")) return null;
	if(word0chtok(str, capt, "case")) return null;
	if(word0chtok(str, capt, "esac")) return null;
	return capt;
})
#define AWSEPS "\"'|&;(){}<> \t\r\n"
DEF_SYMBOL_TERMINAL(assignment, {
	if(!str) return null;
	for(; *str && *str != '='; str++) if(strchr(AWSEPS, *str)) return null;
	if(*str++ != '=') return null;
	return !*str || isblank(*str) ? str : capture_word(str);
})
static string heredoc_(string str, bool remt){
	if(!str) return null;
	const string sstr = str;
	str = capture_word(str);
	if(!str) return null;
	Buffer delim = buffer_new_from(sstr, str-sstr);
	if(delim->data[0] == '\'' || delim->data[0] == '\"'){
		char q = delim->data[0];
		buffer_delete(delim, 0, 1);
		string cq = strchr(delim->data, q);
		if(!cq) str = null;
		else buffer_delete(delim, cq-delim->data, delim->size);
	}
	while(str){
		str = strstr(str, delim->data);
		if(!str){
			hijakmore_ctrl(HIJAKMORE_MRK);
			break;
		}
		string sss = str-1;
		if(remt) for(; sss > sstr && *sss == '\t'; sss--);
		bool laste = sss > sstr && *sss == '\n';
		if(laste && (str[delim->size] == '\0' || str[delim->size] == '\n' || strpref("\r\n", str+delim->size))){
			str += delim->size;
			break;
		}
		str += delim->size;
	}
	buffer_destroy(delim);
	return str;
}
DEF_SYMBOL_TERMINAL(heredoc1, { return heredoc_(str, false); })
DEF_SYMBOL_TERMINAL(heredoc2, { return heredoc_(str, true); })

DEF_SYMBOL_TERMINAL(s0, { return str ? str : null; })
//terminals
#define DEF_SYMBOL_TERMINAL_1xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr && str[1] != chr ? str+1 : null; })
DEF_SYMBOL_TERMINAL_1xCHAR(amp,'&')
DEF_SYMBOL_TERMINAL_1xCHAR(vpip,'|')
DEF_SYMBOL_TERMINAL_1xCHAR(scol,';')
DEF_SYMBOL_TERMINAL_1xCHAR(excl,'!')
DEF_SYMBOL_TERMINAL_1xCHAR(parl,'(')
DEF_SYMBOL_TERMINAL_1xCHAR(parr,')')
ATTR_UNUSED DEF_SYMBOL_TERMINAL_1xCHAR(brakl,'[')
ATTR_UNUSED DEF_SYMBOL_TERMINAL_1xCHAR(brakr,']')
DEF_SYMBOL_TERMINAL_1xCHAR(bracl,'{')
DEF_SYMBOL_TERMINAL_1xCHAR(bracr,'}')
#define DEF_SYMBOL_TERMINAL_2xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr && str[1] == chr ? str+2 : null; })
DEF_SYMBOL_TERMINAL_2xCHAR(ampamp, '&')
DEF_SYMBOL_TERMINAL_2xCHAR(vpipvpip, '|')
DEF_SYMBOL_TERMINAL_2xCHAR(scolscol, ';')


DEF_GROUP(amp_or_scol, RULE(SYMBOL_T(amp)); RULE(SYMBOL_T(scol)))
DEF_GROUP(ampamp_or_vpipvpip, RULE(SYMBOL_T(ampamp)); RULE(SYMBOL_T(vpipvpip)))

#define DEF_SYMBOL_TERMINAL_CSTR(symbl, cstr) DEF_SYMBOL_TERMINAL(symbl, { return str && strpref(cstr, str) ? str+(sizeof(cstr)/sizeof(*cstr)-1) : null; })
#define WSEPS "|&;()<> \t\r\n"
#define DEF_SYMBOL_TERMINAL_CSTRE(symbl, cstr) DEF_SYMBOL_TERMINAL(symbl, { return str && strpref(cstr, str) && strchr(WSEPS, str[(sizeof(cstr)/sizeof(*cstr))-1]) ? str+(sizeof(cstr)/sizeof(*cstr)-1) : null; })
DEF_SYMBOL_TERMINAL_CSTRE(sp_if,	"if")
DEF_SYMBOL_TERMINAL_CSTRE(sp_then,	"then")
DEF_SYMBOL_TERMINAL_CSTRE(sp_elif,	"elif")
DEF_SYMBOL_TERMINAL_CSTRE(sp_else,	"else")
DEF_SYMBOL_TERMINAL_CSTRE(sp_fi,	"fi")
DEF_SYMBOL_TERMINAL_CSTRE(sp_do,	"do")
DEF_SYMBOL_TERMINAL_CSTRE(sp_done,	"done")
DEF_SYMBOL_TERMINAL_CSTRE(sp_for,	"for")
DEF_SYMBOL_TERMINAL_CSTRE(sp_in,	"in")
DEF_SYMBOL_TERMINAL_CSTRE(sp_while,	"while")
DEF_SYMBOL_TERMINAL_CSTRE(sp_until,	"until")
DEF_SYMBOL_TERMINAL_CSTRE(sp_case,	"case")
DEF_SYMBOL_TERMINAL_CSTRE(sp_esac,	"esac")

//\n
#define DEF_GKLEENE(name, r0symbols) DEF_GROUP(name, RULE(r0symbols; SYMBOL_G(name)); RULE(SYMBOL_T(s0)))
#define DEF_GOPT(name, rules) DEF_GROUP(name, rules; RULE(SYMBOL_T(s0)))

DEF_GKLEENE(words, SYMBOL_T(word))

DEF_SYMBOL_TERMINAL(newline, {
	if(!str) return null;
	if(strpref("\r\n", str)) return str+2;
	if(strpref("\n", str)) return str+1;
	return null;
})
DEF_GKLEENE(newlines, SYMBOL_T(newline))

DEF_GROUP(scol_or_newline, RULE(SYMBOL_T(scol)); RULE(SYMBOL_T(newline)))

//fd_t
DEF_SYMBOL_TERMINAL(streamid, {
	if(!str) return null;
	string_mut end = null;
	long sid = strtol(str, &end, 10);
	return end > str && sid >= 0 ? end : null;
})
DEF_GOPT(streamid_opt, RULE(SYMBOL_T(streamid)))

//redirections
DEF_SYMBOL_TERMINAL_1xCHAR(redir_out, '>')
DEF_SYMBOL_TERMINAL_1xCHAR(redir_in, '<')
DEF_SYMBOL_TERMINAL_CSTR(redir_out_append, ">>")
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromheredoc_1, "<<")
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromheredoc_2, "<<-")
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromherestring, "<<<")
DEF_SYMBOL_TERMINAL_CSTR(redir_out_dup, ">&")
DEF_SYMBOL_TERMINAL_CSTR(redir_in_dup, "<&")
DEF_SYMBOL_TERMINAL_CSTR(redir_out_ignorenoclobber, ">|")
DEF_SYMBOL_TERMINAL_CSTR(redir_inout, "<>")

#define redirsymb2type(symb) ( symb == redir_out ? REDIR_OUT : symb == redir_out_append ? REDIR_OUT_APPEND : symb == redir_out_dup ? REDIR_OUT_DUP : symb == redir_out_ignorenoclobber ? REDIR_OUT_CLOBBER : symb == redir_in ? REDIR_IN : symb == redir_in_fromheredoc_1 || symb == redir_in_fromheredoc_2 || symb == redir_in_fromherestring ? REDIR_IN_HERE : symb == redir_in_dup ? REDIR_IN_DUP : symb == redir_inout ? REDIR_INOUT : REDIR_NO )

//TODO add more
DEF_GROUP(redirection,
	RULEp(-10, SYMBOL_G(streamid_opt); SYMBOL_T(redir_out); SYMBOL_T(word));
	RULEp(-10, SYMBOL_G(streamid_opt); SYMBOL_T(redir_in); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_append); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromheredoc_1); SYMBOL_T(heredoc1));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromheredoc_2); SYMBOL_T(heredoc2));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromherestring); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_dup); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_dup); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_ignorenoclobber); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_inout); SYMBOL_T(word));
)
DEF_GKLEENE(redirections, SYMBOL_G(redirection))

//cmd list
static Group cmdlist_l3ext(); //&;\n
static Group cmdlist_l3(); //&;
static Group cmdlist_l2(); //&&||
static Group cmdlist_l1(); //|
static Group command();

DEF_GOPT(amp_or_scol_opt, RULE(SYMBOL_G(amp_or_scol)))
DEF_GOPT(excl_opt, RULE(SYMBOL_T(excl)))
DEF_GKLEENE(cmdlist_l3_r, SYMBOL_G(amp_or_scol); SYMBOL_G(cmdlist_l2))
DEF_GROUP(cmdlist_l3, RULE(SYMBOL_G(cmdlist_l2); SYMBOL_G(cmdlist_l3_r); SYMBOL_G(amp_or_scol_opt)))
DEF_GROUP(cmdlist_l2,
	RULE(SYMBOL_G(cmdlist_l2); SYMBOL_G(ampamp_or_vpipvpip); SYMBOL_G(cmdlist_l1));
	RULE(SYMBOL_G(cmdlist_l1))
)
DEF_GKLEENE(cmdlist_l1_r, SYMBOL_T(vpip); SYMBOL_G(newlines); SYMBOL_G(command))
DEF_GROUP(cmdlist_l1, RULE(SYMBOL_G(excl_opt); SYMBOL_G(command); SYMBOL_G(cmdlist_l1_r)))

DEF_GROUP(cmdlist_l3ext_sep,
	RULE(SYMBOL_T(amp); SYMBOL_G(newlines));
	RULE(SYMBOL_T(scol); SYMBOL_G(newlines));
	RULE(SYMBOL_T(newline); SYMBOL_G(newlines))
)
DEF_GOPT(cmdlist_l3ext_sep_opt, RULE(SYMBOL_G(cmdlist_l3ext_sep)))
DEF_GKLEENE(cmdlist_l3ext_r, SYMBOL_G(cmdlist_l3ext_sep); SYMBOL_G(cmdlist_l2))
DEF_GROUP(cmdlist_l3ext, RULE(SYMBOL_G(newlines); SYMBOL_G(cmdlist_l2); SYMBOL_G(cmdlist_l3ext_r); SYMBOL_G(cmdlist_l3ext_sep_opt)))


//command
static Group cmd_simple();
static Group cmd_compound();
static Group cmd_fundecl();
DEF_GROUP(command, RULE(SYMBOL_G(cmd_simple)); RULE(SYMBOL_G(cmd_compound); SYMBOL_G(redirections)); RULE(SYMBOL_G(cmd_fundecl); SYMBOL_G(redirections)))

DEF_GROUP(cmd_simple_pref, RULE(SYMBOL_T(assignment)); RULE(SYMBOL_G(redirection)))
DEF_GKLEENE(cmd_simple_pref_r, SYMBOL_G(cmd_simple_pref))
DEF_GROUP(cmd_simple_el, RULE(SYMBOL_T(word)); RULE(SYMBOL_G(redirection)))
DEF_GROUP(cmd_simple_el_0, RULE(SYMBOL_T(word_0)); RULE(SYMBOL_G(redirection)))
DEF_GKLEENE(cmd_simple_el_r, SYMBOL_G(cmd_simple_el))
DEF_GROUP(cmd_simple,
	RULE(SYMBOL_G(cmd_simple_pref); SYMBOL_G(cmd_simple_pref_r));
	RULE(SYMBOL_G(cmd_simple_pref_r); SYMBOL_G(cmd_simple_el_0); SYMBOL_G(cmd_simple_el_r))
)

static Group blok_if();
static Group blok_for();
static Group blok_while();
static Group blok_until();
static Group blok_case();

DEF_GROUP(cmd_compound,
	RULE(SYMBOL_T(parl); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(parr));
	RULE(SYMBOL_T(bracl); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(bracr));
	RULE(SYMBOL_G(blok_if));
	RULE(SYMBOL_G(blok_for));
	RULE(SYMBOL_G(blok_while));
	RULE(SYMBOL_G(blok_until));
	RULE(SYMBOL_G(blok_case))
)

DEF_SYMBOL_TERMINAL_CSTRE(sp_function,	"function")
DEF_GOPT(sp_function_opt, RULE(SYMBOL_T(sp_function)))
DEF_GROUP(cmd_fundecl,
	RULE(SYMBOL_G(sp_function_opt); SYMBOL_T(word); SYMBOL_T(parl); SYMBOL_T(parr); SYMBOL_G(newlines); SYMBOL_G(cmd_compound))
)

//blocks
DEF_GOPT(blok_if_else,
	RULE(SYMBOL_T(sp_else); SYMBOL_G(cmdlist_l3ext));
	RULE(SYMBOL_T(sp_elif); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(sp_then); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_if_else))
)
DEF_GROUP(blok_if, RULE(SYMBOL_T(sp_if); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(sp_then); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_if_else); SYMBOL_T(sp_fi)))

DEF_GROUP(blok_do_done, RULE(SYMBOL_T(sp_do); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(sp_done)))

DEF_GROUP(blok_for_list,
	RULE(SYMBOL_G(newlines); SYMBOL_T(sp_in); SYMBOL_G(words); SYMBOL_G(scol_or_newline));
	RULE(SYMBOL_T(scol));
	RULE(SYMBOL_T(s0))
)
DEF_GROUP(blok_for, RULE(SYMBOL_T(sp_for); SYMBOL_T(word); SYMBOL_G(blok_for_list); SYMBOL_G(newlines); SYMBOL_G(blok_do_done)))

DEF_GROUP(blok_while, RULE(SYMBOL_T(sp_while); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_do_done)))
DEF_GROUP(blok_until, RULE(SYMBOL_T(sp_until); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_do_done)))

DEF_GOPT(parl_opt, RULE(SYMBOL_T(parl)))
DEF_GOPT(scolscol_opt, RULE(SYMBOL_T(scolscol)))

DEF_GKLEENE(blok_case_case_rec, SYMBOL_T(vpip); SYMBOL_T(word))
DEF_GOPT(cmdlist_l3ext_opt, RULE(SYMBOL_G(cmdlist_l3ext)))
DEF_GROUP(blok_case_case, RULE(SYMBOL_G(parl_opt); SYMBOL_T(word); SYMBOL_G(blok_case_case_rec); SYMBOL_T(parr); SYMBOL_G(newlines); SYMBOL_G(cmdlist_l3ext_opt)))
DEF_GKLEENE(blok_case_s_rec, SYMBOL_T(scolscol); SYMBOL_G(newlines); SYMBOL_G(blok_case_case))
DEF_GOPT(blok_case_s, RULE(SYMBOL_G(blok_case_case); SYMBOL_G(blok_case_s_rec); SYMBOL_G(scolscol_opt); SYMBOL_G(newlines)))
DEF_GROUP(blok_case, RULE(SYMBOL_T(sp_case); SYMBOL_T(word); SYMBOL_G(newlines); SYMBOL_T(sp_in); SYMBOL_G(newlines); SYMBOL_G(blok_case_s); SYMBOL_T(sp_esac)))

DEF_SYMBOL_TERMINAL(eoi, { return str && !*str ? str : null; })
DEF_GROUP(newline_or_eoi, RULE(SYMBOL_T(newline)); RULE(SYMBOL_T(eoi)))
DEF_GROUP(entry, RULE(SYMBOL_G(cmdlist_l3); SYMBOL_G(newline_or_eoi)); RULE(SYMBOL_G(newline_or_eoi)))

DEF_GRAMMAR(tihs24def,
	GROUP(amp_or_scol);
	GROUP(ampamp_or_vpipvpip);
	GROUP(words);
	GROUP(newlines);
	GROUP(scol_or_newline);
	GROUP(streamid_opt);
	GROUP(redirection);
	GROUP(redirections);
	GROUP(amp_or_scol_opt);
	GROUP(excl_opt);
	GROUP(cmdlist_l3_r);
	GROUP(cmdlist_l3);
	GROUP(cmdlist_l2);
	GROUP(cmdlist_l1_r);
	GROUP(cmdlist_l1);
	GROUP(cmdlist_l3ext_sep);
	GROUP(cmdlist_l3ext_sep_opt);
	GROUP(cmdlist_l3ext_r);
	GROUP(cmdlist_l3ext);
	GROUP(command);
	GROUP(cmd_simple_pref);
	GROUP(cmd_simple_pref_r);
	GROUP(cmd_simple_el);
	GROUP(cmd_simple_el_0);
	GROUP(cmd_simple_el_r);
	GROUP(cmd_simple);
	GROUP(cmd_compound);
	GROUP(sp_function_opt);
	GROUP(cmd_fundecl);
	GROUP(blok_if_else);
	GROUP(blok_if);
	GROUP(blok_do_done);
	GROUP(blok_for_list);
	GROUP(blok_for);
	GROUP(blok_while);
	GROUP(blok_until);
	GROUP(parl_opt);
	GROUP(scolscol_opt);
	GROUP(blok_case_case_rec);
	GROUP(cmdlist_l3ext_opt);
	GROUP(blok_case_case);
	GROUP(blok_case_s_rec);
	GROUP(blok_case_s);
	GROUP(blok_case);
	GROUP(newline_or_eoi);
	GROUP(entry)
) //TODO impatiently waiting for CALP 0.3

#include <calp/parser/build.h>

Parser parcer_defolt_new(void){
	IfElse_T(parser_build(tihs24def()), parser, {
		return parser;
	}, err, { //FIXME should _never_ happen - for removal(?)
		// logerrf("Parser construction failed - %s", err.s);
		return null;
	});
}

#include <calp/parser/fun.h>
#include <calp/lexer.h>
#include <parc24/pars.h>

#define space_comments_skippity(str) for(; *str && (isspace(*str) || *str == '#') && *str != '\n' && *str != '\r'; str++) if(*str == '#'){ for(; *str && *str != '\n'; str++); if(!*str) str--; }

static bool hijakmore_ctrl(enum hijakmore_op op){
	static __thread bool conditionmet = false;
	switch(op){
		case HIJAKMORE_RST: return conditionmet = false;
		case HIJAKMORE_MRK: return conditionmet = true;
		case HIJAKMORE_GET: return conditionmet;
		default: return false;
	}
}

static LexerResult lexer_spacebegone_withcomments(string str, SelfLexingToken tok){
	if(!str) return Error_T(lexer_result, {"Invalid input - null string"});
	space_comments_skippity(str);
	if(tok == newline && !*str) hijakmore_ctrl(HIJAKMORE_MRK);
	string nom = tok(str);
	if(!nom) return Error_T(lexer_result, {"Token refused to eat"});
	string next = nom;
	space_comments_skippity(next);
	if(tok == newline && !*next) hijakmore_ctrl(HIJAKMORE_MRK);
	return Ok_T(lexer_result, {str, nom, next});
}


ParceResult parcer_parse(Parser p, string str, bool cangivemore){
	if(cangivemore) hijakmore_ctrl(HIJAKMORE_RST);
	ParseResult pr = parser_parse(p, lexer_spacebegone_withcomments, str, &entry);
	return IsOk_T(pr) ? Ok_T(parce_result, { pr.r.ok.ast, pr.r.ok.end }) : Error_T(parce_result, { cangivemore && hijakmore_ctrl(HIJAKMORE_GET), pr.r.error });
}

#include <parc24/travast.h>
#include <cppo.h>
#include <unistd.h>
#include <stdlib.h>
#include <util/argsarr_mut.h>

#define isexitcodeok(c) ((c&0xFF) != 0)

#define arrmuttake1(var, arr, clenup) if(!arr || arr->size != 1) retclean(Error_T(travast_result, {"expected a word to expand to one word"}), { argsarrmut_destroy(arr); clenup }); string_mut var = arr->args[0]; arr->args[0] = null; argsarrmut_destroy(arr)

/**
 * @consumes ctxt->ios
 * @param ast @ref
 * @param ctxt @ref
 * @return Result 
 */
static Result traverse_ast_background(AST ast, ParContext ctxt);

TraverseASTResult traverse_ast(AST ast, ParContext ctxt){
	if(ast->type == AST_LEAF){
		const TerminalSymbolId sid = ast->d.leaf.symbolId;
		if(sid == assignment){
			string_mut var = ast->d.leaf.val;
			string_mut eq = strchr(var, '=');
			*eq = '\0';
			if(!eq[1]){
				if(!IsOk(varstore_add(ctxt->vars, var, ""))) retclean(Error_T(travast_result, {"failed to add variable to store"}), { *eq = '='; });
			} else {
				ExpandoResult varva = expando_word(eq+1, ((struct expando_targets){ .tilde = true, .parvar = true, .arithmetics = true, .command = true, .process = true, .path = false, .quot = true }), ctxt);
				if(!IsOk_T(varva)) return Error_T(travast_result, varva.r.error);
				arrmuttake1(varv, varva.r.ok, {});
				if(!IsOk(varstore_add_(ctxt->vars, var, varv))) retclean(Error_T(travast_result, {"failed to add variable to store"}), { *eq = '='; free(varv); });
			}
			*eq = '=';
			return Ok_T(travast_result, {0});
		}
		return Error_T(travast_result, {"AST (leaf) not recognized"});
	}
	const GroupId gid = ast->d.group.groupId;
	if(gid == entry){
		if(ast->d.group.cc > 1) return traverse_ast(ast->d.group.children[0], ctxt);
		else return Ok_T(travast_result, {0});
	}
	//l3: ; & \n
	if(gid == cmdlist_l3 || gid == cmdlist_l3_r){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		const size_t it1 = gid == cmdlist_l3 ? 0 : 1;
		const size_t ir = gid == cmdlist_l3 ? 1 : 2;
		TraverseASTResult t1 = traverse_ast(ast->d.group.children[it1], ctxt);
		if(!IsOk_T(t1)) return t1;
		if(ast->d.group.children[ir]->d.group.cc == 1) return t1;
		t1 = parcontext_uniwait(t1);
		if(!IsOk_T(t1) || travt_is_shrtct(t1.r.ok.type)) return t1;
		ctxt->lastexit = t1.r.ok.v.completed;
		return traverse_ast(ast->d.group.children[ir], ctxt);
	}
	if(gid == cmdlist_l3ext || gid == cmdlist_l3ext_r){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		const size_t it1 = gid == cmdlist_l3ext ? 1 : 1;
		const size_t ir = gid == cmdlist_l3ext ? 2 : 2;
		TraverseASTResult t1 = traverse_ast(ast->d.group.children[it1], ctxt);
		if(!IsOk_T(t1)) return t1;
		if(ast->d.group.children[ir]->d.group.cc == 1) return t1;
		t1 = parcontext_uniwait(t1);
		if(!IsOk_T(t1) || travt_is_shrtct(t1.r.ok.type)) return t1;
		ctxt->lastexit = t1.r.ok.v.completed;
		return traverse_ast(ast->d.group.children[ir], ctxt);
	}
	if(gid == cmdlist_l3ext_opt) return ast->d.group.children[0]->type == AST_GROUP ? traverse_ast(ast->d.group.children[0], ctxt) : Ok_T(travast_result, {0});
	//l2: || &&
	if(gid == cmdlist_l2){
		TraverseASTResult t1 = traverse_ast(ast->d.group.children[0], ctxt);
		if(ast->d.group.cc == 1 || !IsOk_T(t1)) return t1;
		t1 = parcontext_uniwait(t1);
		if(!IsOk_T(t1) || travt_is_shrtct(t1.r.ok.type)) return t1;
		int rc = ctxt->lastexit = t1.r.ok.v.completed;
		return (ast->d.group.children[1]->d.group.children[0]->d.leaf.symbolId == (isexitcodeok(rc) ? ampamp : vpipvpip)) ? t1 : traverse_ast(ast->d.group.children[2], ctxt);
	}
	//l1: |
	if(gid == cmdlist_l1 || gid == cmdlist_l1_r){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		const size_t it1 = gid == cmdlist_l1 ? 1 : 2;
		const size_t ir = gid == cmdlist_l1 ? 2 : 3;
		if(ast->d.group.children[ir]->d.group.cc == 1) return traverse_ast(ast->d.group.children[it1], ctxt);
		struct parcontext cl = *ctxt;
		struct parcontext cr = *ctxt;
		if(!(cl.ios = iosstack_snapdup(cl.ios))) return Error_T(travast_result, {"failed to snapshot IO"});
		if(!(cr.ios = iosstack_snapdup(cr.ios))) return Error_T(travast_result, {"failed to snapshot IO"});
		PipeResult pipe = pipe_new();
		if(!IsOk_T(pipe)) return Error_T(travast_result, pipe.r.error);
		iostack_io_open(cl.ios, IOSTREAM_STD_OUT, pipe.r.ok.write);
		iostack_io_open(cr.ios, IOSTREAM_STD_IN, pipe.r.ok.read);
		Result lr = traverse_ast_background(ast->d.group.children[it1], &cl);
		TraverseASTResult ret = IsOk(lr) ? traverse_ast(ast->d.group.children[ir], &cr) : Error_T(travast_result, {"pipeline background left failed"});
		iosstack_destroy(cr.ios);
		return ret;
	}
	//sparlkes
	if(gid == redirection){
		const TerminalSymbolId assid = ast->d.group.children[1]->d.leaf.symbolId;
		int stream;
		{
			AST sid = ast->d.group.children[0];
			if(sid->d.group.children[0]->d.leaf.symbolId == streamid){
				Str2IResult ion = str2i(sid->d.group.children[0]->d.leaf.val);
				if(!IsOk_T(ion) || ion.r.ok < 0) return Error_T(travast_result, {"invalid IO stream number"});
				stream = ion.r.ok;
			} else stream = -1;
		}
		enum redirection redir = redirsymb2type(assid);
		string_mut target;
		switch(redir){
			case REDIR_NO: return Ok_T(travast_result, {0});
			case REDIR_IN_HERE: {
				string s = ast->d.group.children[2]->d.leaf.val;
				if(assid == redir_in_fromherestring){
					ExpandoResult wexa = expando_word(s, expando_targets_patt, ctxt);
					if(!IsOk_T(wexa)) return Error_T(travast_result, wexa.r.error);
					arrmuttake1(wex, wexa.r.ok, {});
					Buffer buff = buffer_new_from(wex, -1);
					free(wex);
					if(!buff) return Error_T(travast_result, {"herestring append newline failed"});
					buffer_append_str(buff, "\n");
					target = buffer_destr(buff);
				} else {
					bool remtabs = assid == redir_in_fromheredoc_2;
					string ss;
					bool expand;
					size_t dels;
					if(s[0] == '\'' || s[0] == '"'){
						char q = s[0];
						if(!(ss = strchr(s+1, q))) return Error_T(travast_result, {"heredoc delimiter capture failed"});
						dels = ss-s-1;
						expand = false;
					} else {
						if(!(ss = capture_word(s))) return Error_T(travast_result, {"heredoc delimiter capture failed"});
						dels = ss-s;
						expand = true;
					}
					for(; *ss && *ss != '\n'; ss++);
					if(ss[0] == '\n') ss++;
					if(!ss[0]) return Error_T(travast_result, {"heredoc end not found"});
					Buffer buff = buffer_new_from(ss, strlen(ss)-dels);
					if(!buff) return Error_T(travast_result, {"heredoc capture failed"});
					if(remtabs) for(size_t ni = 0; ni < buff->size;){
						size_t nit = ni;
						for(; nit < buff->size && buff->data[nit] == '\t'; nit++);
						buffer_delete(buff, ni, nit);
						for(; ni < buff->size && buff->data[ni] != '\n'; ni++);
						if(ni < buff->size && buff->data[ni] == '\n') ni++;
					}
					if(expand){
						ExpandoResult texa = expando_word(buff->data, expando_targets_parc, ctxt);
						buffer_destroy(buff);
						if(!IsOk_T(texa)) return Error_T(travast_result, texa.r.error);
						arrmuttake1(tex, texa.r.ok, {});
						target = tex;
					} else target = buffer_destr(buff);
				}
				break;
			}
			default: {
				ExpandoResult wexa = expando_word(ast->d.group.children[2]->d.leaf.val, expando_targets_all, ctxt);
				if(!IsOk_T(wexa)) return Error_T(travast_result, wexa.r.error);
				arrmuttake1(wex, wexa.r.ok, {});
				target = wex;
				break;
			} 
		}
		retclean(parcontext_uniredir(redir, stream, target, ctxt), {free(target);});
	}
	if(gid == redirections){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		TraverseASTResult r = traverse_ast(ast->d.group.children[0], ctxt);
		if(!IsOk_T(r)) return r;
		return traverse_ast(ast->d.group.children[1], ctxt);
	}
	//command
	if(gid == command){
		if(ast->d.group.cc > 1){
			TraverseASTResult r = traverse_ast(ast->d.group.children[1], ctxt);
			if(!IsOk_T(r)) return r;
		}
		return traverse_ast(ast->d.group.children[0], ctxt);
	}
	//simple cmd
	if(gid == cmd_simple_pref) return traverse_ast(ast->d.group.children[0], ctxt);
	if(gid == cmd_simple_pref_r){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		TraverseASTResult r1 = traverse_ast(ast->d.group.children[0], ctxt);
		if(!IsOk_T(r1)) return r1;
		return traverse_ast(ast->d.group.children[1], ctxt);
	}
	if(gid == cmd_simple){
		if(ast->d.group.cc == 2){
			//TODO plain sight redirections?
			TraverseASTResult r1 = traverse_ast(ast->d.group.children[0], ctxt);
			if(!IsOk_T(r1)) return r1;
			return traverse_ast(ast->d.group.children[1], ctxt);
		}
		if(ast->d.group.cc == 3){
			const AST c0 = ast->d.group.children[1]->d.group.children[0];
			size_t words = c0->type == AST_LEAF ? 1 : 0;
			for(AST elr = ast->d.group.children[2]; elr->d.group.cc > 1; elr = elr->d.group.children[1]) if(elr->d.group.children[0]->d.group.children[0]->type == AST_LEAF) words++;
			if(words == 0) return Error_T(travast_result, {"don't know what to do with a command without words"}); //FIXME
			struct parcontext c = *ctxt;
			iosstack_push(c.ios);
			#undef cleanup
			#define cleanup { iosstack_pop(c.ios); }
			for(AST ar = ast->d.group.children[0]; ar->d.group.cc > 1; ar = ar->d.group.children[1]){
				TraverseASTResult rass = traverse_ast(ar->d.group.children[0]->d.group.children[0], &c);
				if(!IsOk_T(rass)) return captclean(rass, cleanup);
			}
			ArgsArr_Mut args = argsarrmut_new(words);
			#undef cleanup
			#define cleanup { argsarrmut_destroy(args); iosstack_pop(c.ios); }
			if(!args) return captclean(Error_T(travast_result, {"args construction failed"}), cleanup);
			if(c0->type == AST_LEAF){
				ExpandoResult expra = expando_word(c0->d.leaf.val, (struct expando_targets){ .tilde = true, .parvar = true, .arithmetics = true, .command = true, .process = true, .path = true, .quot = true }, &c);
				if(!IsOk_T(expra)) return captclean(Error_T(travast_result, {"command word expando failed"}), cleanup);
				if(!IsOk(argsarrmut_appendnom(args, expra.r.ok))) return captclean(Error_T(travast_result, {"command word expando failed"}), cleanup);
			} else {
				TraverseASTResult rredir = traverse_ast(c0, &c);
				if(!IsOk_T(rredir)) return captclean(rredir, cleanup);
			}
			{
				for(AST elr = ast->d.group.children[2]; elr->d.group.cc > 1; elr = elr->d.group.children[1]) if(elr->d.group.children[0]->d.group.children[0]->type == AST_LEAF){
					ExpandoResult expra = expando_word(elr->d.group.children[0]->d.group.children[0]->d.leaf.val, (struct expando_targets){ .tilde = true, .parvar = true, .arithmetics = true, .command = true, .process = true, .path = true, .quot = true }, &c);
					if(!IsOk_T(expra)) return captclean(Error_T(travast_result, {"command word expando failed"}), cleanup);
					if(!IsOk(argsarrmut_appendnom(args, expra.r.ok))) return captclean(Error_T(travast_result, {"command word expando failed"}), cleanup);
				} else {
					TraverseASTResult rredir = traverse_ast(elr->d.group.children[0]->d.group.children[0], &c);
					if(!IsOk_T(rredir)) return captclean(rredir, cleanup);
				}
			}
			retclean(parcontext_unixec(args->args, &c), cleanup);
			#undef cleanup
		}
	}
	if(gid == cmd_compound){
		if(ast->d.group.cc == 3){
			if(ast->d.group.children[0]->d.leaf.symbolId == parl){
				struct parcontext cctxt = *ctxt;
				if(!IsOk(parcontext_subco_all(&cctxt))) return Error_T(travast_result, {"failed to subcontextualize"});
				TraverseASTResult rtra = traverse_ast(ast->d.group.children[1], &cctxt);
				parcontext_subco_destroy(&cctxt);
				if(!IsOk_T(rtra)) return rtra;
				if(rtra.r.ok.type == TRAV_SHRTCT_EXIT) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = rtra.r.ok.v.completed}});
				if(travt_is_shrtct(rtra.r.ok.type)) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
				return rtra;
			}
			return traverse_ast(ast->d.group.children[1], ctxt);
		}
		if(ast->d.group.cc == 1) return traverse_ast(ast->d.group.children[0], ctxt);
	}
	if(gid == cmd_fundecl){
		string name = ast->d.group.children[1]->d.leaf.val;
		AST fast = ast->d.group.children[5];
		if(!IsOk(funcstore_add(ctxt->funcs, name, fast))) return Error_T(travast_result, {"failed to add to function store"});
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
	}
	//blocks
	if(gid == blok_if || gid == blok_if_else){
		if(ast->d.group.cc == 1) return Ok_T(travast_result, {0});
		if(ast->d.group.cc == 2) return traverse_ast(ast->d.group.children[1], ctxt);
		TraverseASTResult cond = parcontext_uniwait(traverse_ast(ast->d.group.children[1], ctxt));
		if(!IsOk_T(cond) || travt_is_shrtct(cond.r.ok.type)) return cond;
		return traverse_ast(ast->d.group.children[cond.r.ok.v.completed == 0 ? 3 : 4], ctxt);
	}
	if(gid == blok_do_done) return traverse_ast(ast->d.group.children[1], ctxt);
	#define chkshrtctexit(res) \
		if(res.r.ok.type == TRAV_SHRTCT_EXIT) break;\
		if(res.r.ok.type == TRAV_SHRTCT_BREAK){ res.r.ok.v.shortcut_depth--; break; }\
		if(res.r.ok.type == TRAV_SHRTCT_CONTINUE){ if(res.r.ok.v.shortcut_depth > 1){ res.r.ok.v.shortcut_depth--; break; } else res = Ok_T(travast_result, {TRAV_COMPLETED, {.completed = ctxt->lastexit}}); }\
		else if(travt_is_hascode(res.r.ok.type) && !IsOk(isexitcodeok(res.r.ok.v.completed))) break
	if(gid == blok_while || gid == blok_until){
		Result brekon = gid == blok_while ? Error : Ok;
		TraverseASTResult res = Ok_T(travast_result, {0});
		ctxt->lupdepth++;
		do {
			TraverseASTResult ec = parcontext_uniwait(traverse_ast(ast->d.group.children[1], ctxt));
			if(!IsOk_T(ec)) retclean(ec, { ctxt->lupdepth--; });
			if(travt_is_shrtct(ec.r.ok.type)) retclean(ec, { ctxt->lupdepth--; }); //TODO hmmmm...
			if(isexitcodeok(ec.r.ok.v.completed) == brekon) break;
			if(!IsOk_T((res = parcontext_uniwait(traverse_ast(ast->d.group.children[2], ctxt))))) break;
			chkshrtctexit(res);
		} while(true);
		if(travt_is_shrtct(res.r.ok.type) && res.r.ok.type != TRAV_SHRTCT_EXIT && res.r.ok.v.shortcut_depth == 0) res = Ok_T(travast_result, {TRAV_COMPLETED, {.completed = ctxt->lastexit}});
		ctxt->lupdepth--;
		return res;
	}
	if(gid == blok_for){
		AST inli = ast->d.group.children[2];
		bool iterargs = inli->d.group.cc == 1;
		AST nvw = iterargs ? null : inli->d.group.children[2];
		const ExpandoResult varnr = expando_word(ast->d.group.children[1]->d.leaf.val, expando_targets_all, ctxt);
		if(!IsOk_T(varnr)) return Error_T(travast_result, {"failed to resolve variable name"});
		arrmuttake1(varn, varnr.r.ok, {});
		ctxt->lupdepth++;
		TraverseASTResult res = Ok_T(travast_result, {0});
		if(iterargs) for(argsarr arg = ctxt->args; *arg; arg++){
			if(!IsOk_T((res = parcontext_uniwait(res)))) break;
			chkshrtctexit(res);
			if(!IsOk(varstore_add(ctxt->vars, varn, *arg))){ res = Error_T(travast_result, {"varstore add failed"}); break; }
			res = traverse_ast(ast->d.group.children[4], ctxt);
			chkshrtctexit(res);
		} else do {
			if(!IsOk_T((res = parcontext_uniwait(res)))) break;
			ExpandoResult nva = expando_word(nvw->d.group.children[0]->d.leaf.val, expando_targets_all, ctxt);
			if(!IsOk_T(nva)){ res = Error_T(travast_result, {"in list element expando failed"}); break; }
			for(size_t i = 0; i < nva.r.ok->size; i++){
				if(!IsOk_T((res = parcontext_uniwait(res)))) break;
				chkshrtctexit(res);
				if(!IsOk(varstore_add(ctxt->vars, varn, nva.r.ok->args[i]))){ res = Error_T(travast_result, {"varstore add failed"}); break; }
				res = traverse_ast(ast->d.group.children[4], ctxt);
				chkshrtctexit(res);
			}
			argsarrmut_destroy(nva.r.ok);
			if(!IsOk_T(res) || travt_is_shrtct(res.r.ok.type)) break;
			nvw = nvw->d.group.children[1];
		} while(nvw->d.group.cc > 1);
		free(varn);
		if(travt_is_shrtct(res.r.ok.type) && res.r.ok.type != TRAV_SHRTCT_EXIT && res.r.ok.v.shortcut_depth == 0) res = Ok_T(travast_result, {TRAV_COMPLETED, {.completed = ctxt->lastexit}});
		ctxt->lupdepth--;
		return res;
	}
	#undef chkshrtctexit
	if(gid == blok_case){
		AST cases = ast->d.group.children[5];
		if(cases->d.group.cc == 1) return Ok_T(travast_result, {TRAV_COMPLETED, {.completed=0}});
		ExpandoResult matchvra = expando_word(ast->d.group.children[1]->d.leaf.val, expando_targets_all, ctxt);
		if(!IsOk_T(matchvra)) return Error_T(travast_result, {"failed to resolve matching target"});
		arrmuttake1(mv, matchvra.r.ok, {});
		AST cc = cases->d.group.children[0];
		do {
			ExpandoResult ccp0a = expando_word(cc->d.group.children[1]->d.leaf.val, expando_targets_patt, ctxt);
			if(!IsOk_T(ccp0a)) return captclean(Error_T(travast_result, {"failed to expand case pattern"}), {free(mv);});
			arrmuttake1(ccp0, ccp0a.r.ok, {});
			PatCompResult patr = pattern_compile(ctxt->patcomp, ccp0, *ctxt->parcopts);
			free(ccp0);
			if(!IsOk_T(patr)) return Error_T(travast_result, {"invalid case pattern"});
			bool ccmatch = pattern_test(patr.r.ok, mv);
			pattern_destroy(patr.r.ok);
			for(AST ccpn = cc->d.group.children[2]; !ccmatch && ccpn->d.group.cc > 1; ccpn = ccpn->d.group.children[2]){
				ExpandoResult pna = expando_word(ccpn->d.group.children[1]->d.leaf.val, expando_targets_all, ctxt);
				if(!IsOk_T(pna)) return captclean(Error_T(travast_result, {"failed to expand case pattern"}), {free(mv);});
				arrmuttake1(pn, pna.r.ok, {free(mv);});
				PatCompResult patr = pattern_compile(ctxt->patcomp, pn, *ctxt->parcopts);
				free(pn);
				if(!IsOk_T(patr)) retclean(Error_T(travast_result, {"invalid case pattern"}), {free(mv);});
				ccmatch |= pattern_test(patr.r.ok, mv);
				pattern_destroy(patr.r.ok);
			}
			if(ccmatch){
				free(mv);
				return traverse_ast(cc->d.group.children[5], ctxt);
			}
			if((cases = cases->d.group.children[cases->d.group.groupId == blok_case_s_rec ? 3 : 1])->d.group.cc <= 1) break;
			cc = cases->d.group.children[2];
		} while(true);
		free(mv);
		return Ok_T(travast_result, {TRAV_COMPLETED, {.completed = 0}});
	}
	// ctxt->io.log(LL_ERROR, "Failed to recognize group: %s", ast->d.group.group->name);
	return Error_T(travast_result, {"AST (group) not recognized"});
}

#include <cppo/parallels.h>
#include <parc24/ioslog.h>

struct travastbgrargs {
	AST ast;
	ParContext ctxt;
};
typedef struct travastbgrargs* TraverseASTBackgroundArgs;

static void* traverse_ast_background_(void* a){
	TraverseASTBackgroundArgs args = a;
	TraverseASTResult r = parcontext_uniwait(traverse_ast(args->ast, args->ctxt));
	if(!IsOk_T(r)) parciolog(args->ctxt->ios, LL_ERROR, "Background traversal error: %s", r.r.error);
	parcontext_subco_destroy(args->ctxt);
	iosstack_destroy(args->ctxt->ios);
	free(args->ctxt);
	ast_destroy(args->ast);
	free(args);
	return null;
}

static Result traverse_ast_background(AST ast, ParContext ctxt){
	TraverseASTBackgroundArgs args = malloc(sizeof(*args));
	if(!args) return Error;
	AST dast = ast_clone(ast);
	if(!dast) retclean(Error, { free(args); iosstack_destroy(ctxt->ios); });
	ParContext cctxt = malloc(sizeof(*cctxt));
	if(!cctxt) retclean(Error, { ast_destroy(dast); free(args); iosstack_destroy(ctxt->ios); });
	*cctxt = *ctxt;
	if(!IsOk(parcontext_subco_all(cctxt))) retclean(Error, { ast_destroy(dast); free(cctxt); free(args); iosstack_destroy(ctxt->ios); });
	*args = (struct travastbgrargs){dast, cctxt};
	return parallels_runf(traverse_ast_background_, args, true).result;
}
