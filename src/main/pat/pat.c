#include "patpars.h"

#include <util/null.h>
#include <stdlib.h>

PatternCompiler patcomp_new(void){
	new(PatternCompiler, c);
	*c = (struct pattern_compiler){patpars_new(), patpars_ext_new()};
	return c;
}

void patcomp_destroy(PatternCompiler pc){
	if(!pc) return;
	parser_destroy(pc->standard);
	parser_destroy(pc->extended);
	free(pc);
}

void pattern_destroy(Pattern p){
	auto_destroy(p);
}

PatCompResult pattern_compile(PatternCompiler pc, string pat, struct parc_options opts){
	if(!pc || !pat) return Error_T(patcomp_result, {"invalid arguments"});
	return (opts.extglob ? pattern_parse_ext : pattern_parse)(opts.extglob ? pc->extended : pc->standard, pat, false /* y the excuse me fork did i implement it then? */);
}

bool pattern_test(Pattern p, string str){
	return auto_test(p, str);
}
