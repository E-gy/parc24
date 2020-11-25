#include <lib.h>

#include <stdlib.h>
#include <string.h>

char* sayhi(const char* lewhom){
	const char* pre = "Hello there, ";
	const size_t prel = strlen(pre);
	const char* inf = lewhom && *lewhom ? lewhom : "General Kenobi";
	const size_t infl = strlen(inf);
	const char* suff = "!~~\\(-v-)/~~|";
	const size_t suffl = strlen(suff);
	char* nstr = calloc(prel+infl+suffl+1, sizeof(*nstr));
	if(!nstr) return NULL;
	memcpy(nstr, pre, prel);
	memcpy(nstr+prel, inf, infl);
	memcpy(nstr+prel+infl, suff, suffl);
	return nstr;
}
