#include <calp/lexer.h>

#include <ctype.h>

LexerResult lexer0(string str, SelfLexingToken tok){
	string nom = tok(str);
	if(nom) return Ok_T(lexer_result, {str, nom, nom});
	return Error_T(lexer_result, {"Token refused to eat"});
}

#define space_skippity(str) for(; *str && isspace(*str) && *str != '\n' && *str != '\r'; str++)

LexerResult lexer_spacebegone(string str, SelfLexingToken tok){
	if(!str) return Error_T(lexer_result, {"Invalid input - null string"});
	space_skippity(str);
	string nom = tok(str);
	if(!nom) return Error_T(lexer_result, {"Token refused to eat"});
	string next = nom;
	space_skippity(next);
	return Ok_T(lexer_result, {str, nom, next});
}
