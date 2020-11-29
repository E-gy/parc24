#include <calp/grammar/define.h>

#include "quotexpando.h"
#include <util/null.h>
#include <util/string.h>
#include <util/buffer.h>
#include <ctype.h>

//what is a word? - whatver that deserves an entire header on its own
DEF_SYMBOL_TERMINAL(word, {
	return capture_word(str);
});
#define AWSEPS "\"'|&;(){}<> \t\r\n"
DEF_SYMBOL_TERMINAL(assignment, {
	if(!str) return null;
	for(; *str && *str != '='; str++) if(strchr(AWSEPS, *str)) return null;
	if(*str++ != '=') return null;
	return capture_word(str);
});
DEF_SYMBOL_TERMINAL(heredoc, {
	if(!str) return null;
	const string sstr = str;
	str = capture_word(str);
	if(!str) return null;
	Buffer delim = buffer_new_from(sstr, str-sstr); //TODO quote expansion on delim
	while(str && *str){
		str = strstr(str, delim->data);
		if(!str) break;
		if(str > sstr && str[-1] == '\n' && (str[delim->size] == '\0' || str[delim->size] == '\n' || strpref("\r\n", str+delim->size))){
			str += delim->size;
			break;
		}
		str += delim->size;
	}
	buffer_destroy(delim);
	return str;
});

DEF_SYMBOL_TERMINAL(s0, { return str ? str : null; });
//terminals
#define DEF_SYMBOL_TERMINAL_1xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr ? str+1 : null; });
DEF_SYMBOL_TERMINAL_1xCHAR(amp,'&');
DEF_SYMBOL_TERMINAL_1xCHAR(vpip,'|');
DEF_SYMBOL_TERMINAL_1xCHAR(scol,';');
DEF_SYMBOL_TERMINAL_1xCHAR(excl,'!');
DEF_SYMBOL_TERMINAL_1xCHAR(parl,'(');
DEF_SYMBOL_TERMINAL_1xCHAR(parr,')');
DEF_SYMBOL_TERMINAL_1xCHAR(brakl,'[');
DEF_SYMBOL_TERMINAL_1xCHAR(brakr,']');
DEF_SYMBOL_TERMINAL_1xCHAR(bracl,'{');
DEF_SYMBOL_TERMINAL_1xCHAR(bracr,'}');
#define DEF_SYMBOL_TERMINAL_2xCHAR(symbl, chr) DEF_SYMBOL_TERMINAL(symbl, { return str && str[0] == chr && str[1] == chr ? str+2 : null; });
DEF_SYMBOL_TERMINAL_2xCHAR(ampamp, '&');
DEF_SYMBOL_TERMINAL_2xCHAR(vpipvpip, '|');
DEF_SYMBOL_TERMINAL_2xCHAR(scolscol, ';');


DEF_GROUP(amp_or_scol, RULE(SYMBOL_T(amp)); RULE(SYMBOL_T(scol)));
DEF_GROUP(ampamp_or_vpipvpip, RULE(SYMBOL_T(ampamp)); RULE(SYMBOL_T(vpipvpip)));

#define DEF_SYMBOL_TERMINAL_CSTR(symbl, cstr) DEF_SYMBOL_TERMINAL(symbl, { return str && strpref(cstr, str) ? str+(sizeof(cstr)/sizeof(*cstr)-1) : null; });
#define WSEPS "|&;()<> \t\r\n"
#define DEF_SYMBOL_TERMINAL_CSTRE(symbl, cstr) DEF_SYMBOL_TERMINAL(symbl, { return str && strpref(cstr, str) && strchr(WSEPS, str[(sizeof(cstr)/sizeof(*cstr))-1]) ? str+(sizeof(cstr)/sizeof(*cstr)) : null; });
DEF_SYMBOL_TERMINAL_CSTRE(sp_if,	"if");
DEF_SYMBOL_TERMINAL_CSTRE(sp_then,	"then");
DEF_SYMBOL_TERMINAL_CSTRE(sp_elif,	"elif");
DEF_SYMBOL_TERMINAL_CSTRE(sp_else,	"else");
DEF_SYMBOL_TERMINAL_CSTRE(sp_fi,	"fi");
DEF_SYMBOL_TERMINAL_CSTRE(sp_do,	"do");
DEF_SYMBOL_TERMINAL_CSTRE(sp_done,	"done");
DEF_SYMBOL_TERMINAL_CSTRE(sp_for,	"for");
DEF_SYMBOL_TERMINAL_CSTRE(sp_in,	"in");
DEF_SYMBOL_TERMINAL_CSTRE(sp_while,	"while");
DEF_SYMBOL_TERMINAL_CSTRE(sp_until,	"until");
DEF_SYMBOL_TERMINAL_CSTRE(sp_case,	"case");
DEF_SYMBOL_TERMINAL_CSTRE(sp_esac,	"esac");

//\n
#define DEF_GKLEENE(name, r0symbols) DEF_GROUP(name, RULE(r0symbols; SYMBOL_G(name)); RULE(SYMBOL_T(s0)))
#define DEF_GOPT(name, rules) DEF_GROUP(name, rules; RULE(SYMBOL_T(s0)))

DEF_SYMBOL_TERMINAL(newline, {
	if(!str) return null;
	if(strpref("\r\n", str)) return str+2;
	if(strpref("\n", str)) return str+1;
	return null;
});
DEF_GKLEENE(newlines, SYMBOL_T(newline))
//fd_t
DEF_SYMBOL_TERMINAL(streamid, {
	if(!str) return null;
	return *str == '0' || *str == '1' || *str == '2' ? str+1 : null; //TODO more streams support?
});
DEF_GOPT(streamid_opt, RULE(SYMBOL_T(streamid)));

//redirections
DEF_SYMBOL_TERMINAL_1xCHAR(redir_out, '>');
DEF_SYMBOL_TERMINAL_1xCHAR(redir_in, '<');
DEF_SYMBOL_TERMINAL_CSTR(redir_out_append, ">>");
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromheredoc_1, "<<");
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromheredoc_2, "<<-");
DEF_SYMBOL_TERMINAL_CSTR(redir_in_fromherestring, "<<<");
DEF_SYMBOL_TERMINAL_CSTR(redir_out_dup, ">&");
DEF_SYMBOL_TERMINAL_CSTR(redir_in_dup, "<&");
DEF_SYMBOL_TERMINAL_CSTR(redir_out_ignorenoclobber, ">|");
DEF_SYMBOL_TERMINAL_CSTR(redir_inout, "<>");

//TODO add more
DEF_GROUP(redirection,
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_append); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromheredoc_1); SYMBOL_T(heredoc));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromheredoc_2); SYMBOL_T(heredoc));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_fromherestring); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_dup); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_in_dup); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_out_ignorenoclobber); SYMBOL_T(word));
	RULE(SYMBOL_G(streamid_opt); SYMBOL_T(redir_inout); SYMBOL_T(word));
);
DEF_GKLEENE(redirections, SYMBOL_G(redirection));

//cmd list
static Group cmdlist_l3ext(); //&;\n
static Group cmdlist_l3(); //&;
static Group cmdlist_l2(); //&&||
static Group cmdlist_l1(); //|
static Group command();

DEF_GOPT(amp_or_scol_opt, RULE(SYMBOL_G(amp_or_scol)));
DEF_GOPT(excl_opt, RULE(SYMBOL_T(excl)));
DEF_GKLEENE(cmdlist_l3_r, SYMBOL_G(amp_or_scol); SYMBOL_G(cmdlist_l2));
DEF_GROUP(cmdlist_l3, RULE(SYMBOL_G(cmdlist_l2); SYMBOL_G(cmdlist_l3_r); SYMBOL_G(amp_or_scol_opt)));
DEF_GKLEENE(cmdlist_l2_r, SYMBOL_G(ampamp_or_vpipvpip); SYMBOL_G(newlines); SYMBOL_G(cmdlist_l1));
DEF_GROUP(cmdlist_l2, RULE(SYMBOL_G(cmdlist_l1); SYMBOL_G(cmdlist_l2_r)));
DEF_GKLEENE(cmdlist_l1_r, SYMBOL_T(vpip); SYMBOL_G(newlines); SYMBOL_G(command));
DEF_GROUP(cmdlist_l1, RULE(SYMBOL_G(excl_opt); SYMBOL_G(command); SYMBOL_G(cmdlist_l1_r)));

DEF_GROUP(cmdlist_l3ext_sep,
	RULE(SYMBOL_T(amp); SYMBOL_G(newlines));
	RULE(SYMBOL_T(scol); SYMBOL_G(newlines));
	RULE(SYMBOL_T(newline); SYMBOL_G(newlines))
);
DEF_GOPT(cmdlist_l3ext_sep_opt, RULE(SYMBOL_G(cmdlist_l3ext_sep)));
DEF_GKLEENE(cmdlist_l3ext_r, SYMBOL_G(cmdlist_l3ext_sep); SYMBOL_G(cmdlist_l2));
DEF_GROUP(cmdlist_l3ext, RULE(SYMBOL_G(newlines); SYMBOL_G(cmdlist_l2); SYMBOL_G(cmdlist_l3ext_r); SYMBOL_G(cmdlist_l3ext_sep_opt)));


//command
static Group cmd_simple();
static Group cmd_compound();
static Group cmd_fundecl();
DEF_GROUP(command, RULE(SYMBOL_G(cmd_simple)); RULE(SYMBOL_G(cmd_compound); SYMBOL_G(redirections)); RULE(SYMBOL_G(cmd_fundecl); SYMBOL_G(redirections)))

DEF_GROUP(cmd_simple_pref, RULE(SYMBOL_T(assignment)); RULE(SYMBOL_G(redirection)));
DEF_GKLEENE(cmd_simple_pref_r, SYMBOL_G(cmd_simple_pref));
DEF_GROUP(cmd_simple_el, RULE(SYMBOL_T(word)); RULE(SYMBOL_G(redirection)));
DEF_GKLEENE(cmd_simple_el_r, SYMBOL_G(cmd_simple_el));
DEF_GROUP(cmd_simple,
	RULE(SYMBOL_G(cmd_simple_pref); SYMBOL_G(cmd_simple_pref_r));
	RULE(SYMBOL_G(cmd_simple_pref_r); SYMBOL_G(cmd_simple_el); SYMBOL_G(cmd_simple_el_r))
);

static Group blok_if();
//TODO more blocks

DEF_GROUP(cmd_compound,
	RULE(SYMBOL_T(parl); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(parr));
	RULE(SYMBOL_T(bracl); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(bracr));
	RULE(SYMBOL_G(blok_if))
);

DEF_SYMBOL_TERMINAL_CSTRE(sp_function,	"function");
DEF_GOPT(sp_function_opt, RULE(SYMBOL_T(sp_function)));
DEF_GROUP(cmd_fundecl,
	RULE(SYMBOL_G(sp_function_opt); SYMBOL_T(word); SYMBOL_T(parl); SYMBOL_T(parr); SYMBOL_G(newlines); SYMBOL_G(cmd_compound))
);

//blocks
DEF_GOPT(blok_if_else,
	RULE(SYMBOL_T(sp_else); SYMBOL_G(cmdlist_l3ext));
	RULE(SYMBOL_T(sp_elif); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(sp_then); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_if_else))
);
DEF_GROUP(blok_if, RULE(SYMBOL_T(sp_if); SYMBOL_G(cmdlist_l3ext); SYMBOL_T(sp_then); SYMBOL_G(cmdlist_l3ext); SYMBOL_G(blok_if_else); SYMBOL_T(sp_fi)));
//TODO add more

DEF_SYMBOL_TERMINAL(eoi, { return str && !*str ? str : null; });
DEF_GROUP(newline_or_eoi, RULE(SYMBOL_T(newline)); RULE(SYMBOL_T(eoi)));
DEF_GROUP(entry, RULE(SYMBOL_G(cmdlist_l3); SYMBOL_G(newline_or_eoi)); RULE(SYMBOL_G(newline_or_eoi)));

DEF_GRAMMAR(tihs24def,
	GROUP(amp_or_scol);
	GROUP(ampamp_or_vpipvpip);
	GROUP(newlines);
	GROUP(streamid_opt);
	GROUP(redirection);
	GROUP(redirections);
	GROUP(amp_or_scol_opt);
	GROUP(excl_opt);
	GROUP(cmdlist_l3_r);
	GROUP(cmdlist_l3);
	GROUP(cmdlist_l2_r);
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
	GROUP(cmd_simple_el_r);
	GROUP(cmd_simple);
	GROUP(cmd_compound);
	GROUP(sp_function_opt);
	GROUP(cmd_fundecl);
	GROUP(blok_if_else);
	GROUP(blok_if);
	GROUP(newline_or_eoi);
	GROUP(entry)
); //TODO impatiently waiting for CALP 0.3

#include <calp/parser/build.h>
#include <util/log.h>

Parser parcer_defolt_new(void){
	IfElse_T(parser_build(tihs24def()), parser, {
		return parser;
	}, err, { //FIXME should _never_ happen - for removal(?)
		logerrf("Parser construction failed - %s", err.s);
		return null;
	});
}

#include <calp/parser/fun.h>
#include <calp/lexers.h>

ParseResult parcer_parse(Parser p, string str){
	return parser_parse(p, lexer_spacebegone, str, &entry);
}
