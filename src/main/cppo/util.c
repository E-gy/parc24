#include <cppo.h>
#include "util.h"

#include <util/null.h>
#include <util/buffer.h>
#include <util/string.h>
#include <ctype.h>

#define SQRS "'\\''"
#define SQRL (sizeof(SQRS)/sizeof(*SQRS))
#ifdef _WIN32
#define SQRosS "''"
#else
#define SQRosS SQRS
#endif
#define SQRosL (sizeof(SQRosS)/sizeof(*SQRosS))
#define WSEPS "\"'|&;()<> \t"

string_mut exe_args_join(string args[], bool os){
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
			for(string s = *arg; *s; s++) nl += *s == '\'' ? (os ? SQRosL : SQRL) : 1;
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
					buffer_append_str(aj, os ? SQRosS : SQRS);
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

#define isescaped(str, s0) __extension__({ bool _esc = false; for(string _s = str-1; _s >= s0 && *_s == '\\'; _s--) _esc = !_esc; _esc; })

ArgsArr_Mut exe_args_split(string cmd){
	if(!cmd) return null;
	ArgsArr_Mut args = argsarrmut_new(16);
	if(!args) return null;
	for(string s = cmd; *s;){
		Buffer buff = buffer_new(64);
		if(!buff) retclean(null, {argsarrmut_destroy(args);});
		string srp = s;
		while(s && *s && !isspace(*s)) switch(*s){
			case '\'': {
				buffer_append(buff, srp, s-srp);
				string clo = strchr(s+1, '\'');
				if(!clo) retclean(null, {buffer_destroy(buff); argsarrmut_destroy(args);});
				buffer_append(buff, s+1, clo-s-1);
				if(strpref(SQRS, clo)){
					buffer_append_str(buff, "'");
					srp = s = clo+3;
				} else srp = s = clo+1;
				break;
			}
			case '"': { //do we _actually_ need this like really?
				buffer_append(buff, srp, s-srp);
				string clo = s+1;
				do {
					clo = strchr(clo+1, '"');
					if(!clo) retclean(null, {buffer_destroy(buff); argsarrmut_destroy(args);});
				} while(isescaped(clo, s));
				buffer_append(buff, s+1, clo-s-1); //TODO escapes if we need this section at all i guess
				srp = s = clo+1;
				break;
			}
			default: s++;
		}
		buffer_append(buff, srp, s-srp);
		if(!IsOk(argsarrmut_append(args, buffer_destr(buff)))) retclean(null, {argsarrmut_destroy(args);});
		while(isspace(*s)) s++;
	}
	return args;
}

#ifdef _WIN32

#include <stdlib.h>

string user_get_home(string user){
	return getenv("USERPROFILE");
}

#else

#include <unistd.h>
#include <pwd.h>

string user_get_home(string user){
	uid_t uid = getuid();
	if(user){
		while(true){
			passwd* entry = getpwent();
			if(!entry) break;
			if(streq(entry->pw_name, user)){
				uid = entry->pw_uid;
				break;
			}
		}
		endpwent();
	}
	return getpwuid(uid)->pw_dir;
}

#endif
