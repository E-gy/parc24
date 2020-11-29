#include "quotexpando.h"

#include <util/string.h>
#include <util/null.h>

#define isescaped(str, s0) __extension__({ bool _esc = false; for(string _s = str-1; _s >= s0 && *_s == '\\'; _s--) _esc = !_esc; _esc; })

string capture_quot(const string str){
	if(!capture_isquotstart(str)) return null;
	if(str[0] == '\''){
		string e = strchr(str+1, '\'');
		return e ? e+1 : null;
	}
	if(str[0] == '"'){
		string quot = str;
		do {
			quot = strchr(quot+1, '"');
			if(!quot) return null;
		} while(isescaped(quot, str));
		for(string s = str+1; s < quot;){
			if(capture_isexpandostart(s) && !isescaped(s, str)){
				string expend = capture_expando(s);
				if(!expend) return null;
				quot = strchr(expend, '"');
				if(!quot) return null;
				s = expend;
			} else s++;
		}
		return quot+1;
	}
	return null;
}

string capture_expando(const string str){
	if(!str) return null;
	if(strpref("$(", str)){
		int bal = 1;
		string s = str+2;
		while(s && *s && bal > 0){
			if(capture_isquotstart(s) && !isescaped(s, str)) s = capture_quot(str);
			else if(capture_isexpandostart(s) && !isescaped(s, str)) s = capture_expando(s);
			else {
				if(!isescaped(s, str)) bal += *s == '(' ? 1 : *s == ')' ? -1 : 0;
				s++;
			}
		}
		return bal == 0 ? s : null;
	}
	if(str[0] == '`'){
		string quot = str;
		do {
			quot = strchr(quot+1, 'q');
			if(!quot) return null;
		} while(isescaped(quot, str));
		return quot;
	}
	return null;
}

#define WSEPS "\"'|&;()<> \t\r\n"

string capture_word(const string str){
	if(!str || !*str) return null;
	string s = str;
	while(s && *s){
		if(capture_isquotstart(s) && !isescaped(s, str)) s = capture_quot(s);
		else if(capture_isexpandostart(s) && !isescaped(s, str)) s = capture_expando(s);
		else if(strchr(WSEPS, *s) && !isescaped(s, str)) break;
		else s++;
	}
	return s > str ? s : null;
}