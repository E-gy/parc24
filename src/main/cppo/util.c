#include <cppo.h>

#include <util/null.h>
#include <util/buffer.h>
#include <util/string.h>

#ifdef _WIN32
#define SQRS "''"
#else
#define SQRS "'\\''"
#endif
#define SQRL (sizeof(SQRS)/sizeof(*SQRS))
#define WSEPS "\"'|&;()<> \t"

string_mut exe_args_join(string args[]){
	if(!args) return null;
	Buffer j = buffer_new(255);
	if(!j) return null;
	Buffer aj = null;
	for(string* arg = args; *arg; arg++){
		if(j->size > 0) buffer_append_str(j, " ");
		bool ahasep = false;
		for(string ms = WSEPS; *ms && !ahasep; ms++) ahasep |= !!strchr(*arg, *ms); 
		if(ahasep){
			size_t nl = 2;
			for(string s = *arg; *s; s++) nl += *s == '\'' ? SQRL : 1;
			if(aj) buffer_resize(aj, nl);
			else aj = buffer_new(nl);
			buffer_append_str(aj, "'");
			for(string s = *arg; *s;){
				string sn = strchr(s, '\'');
				if(!sn){
					buffer_append_str(aj, s);
					break;
				} else {
					buffer_append(aj, s, sn-s);
					buffer_append_str(aj, SQRS);
					s = sn+1;
				}
			}
			buffer_append_str(aj, "'");
			buffer_append_str(j, aj->data);
			aj->data[0] = '\0';
			aj->size = 0;
		} else buffer_append_str(j, *arg);
	}
	buffer_destroy(aj);
	return buffer_destr(j);
}
