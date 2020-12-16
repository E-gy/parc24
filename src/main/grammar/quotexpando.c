#include "quotexpando.h"

#include <util/string.h>
#include <util/null.h>
#include <util/buffer.h>
#include <util/flist.h>
#include <tihs/exe.h>
#include <cppo.h>
#include <cppo/parallels.h>
#include <stdlib.h>
#include <unistd.h>

#define isescaped(str, s0) __extension__({ bool _esc = false; for(string _s = str-1; _s >= s0 && *_s == '\\'; _s--) _esc = !_esc; _esc; })

#define str (buff->data+(*si))
#define s (buff->data+i)

ExpandoResult expando_quot(Buffer buff, size_t* si, struct expando_targets what, ParContext context);
ExpandoResult expando_expando(Buffer buff, size_t* si, struct expando_targets what, ParContext context);
ExpandoResult expando_variable(Buffer buff, size_t* si, struct expando_targets what, ParContext context);
ExpandoResult expando_tilde(Buffer buff, size_t* si, struct expando_targets what, ParContext context);

ExpandoResult expando_quot(Buffer buff, size_t* si, struct expando_targets what, ParContext context){
	if(str[0] == '\''){
		string e = strchr(str+1, '\'');
		if(!e) return Error_T(expando_result, {"didn't fint matching `'`"});
		size_t ei = e-buff->data;
		buffer_delete(buff, ei, ei+1);
		buffer_delete(buff, *si, (*si)+1);
		*si = ei-1;
		return Ok_T(expando_result, null);
	}
	if(str[0] == '\"'){
		what.path = false;
		size_t i = *si;
		buffer_delete(buff, i, i+1);
		while(true){
			string nquot = strchr(s, '"');
			if(!nquot) return Error_T(expando_result, {"didn't fint matching '\"'"});
			size_t ei = nquot-buff->data;
			while(i <= ei){
				if(i == ei){
					buffer_delete(buff, i, i+1);
					*si = i;
					return Ok_T(expando_result, null);
				}
				if(capture_isexpandostart(s)){
					IfError_T(expando_expando(buff, &i, what, context), err, { return Error_T(expando_result, err); });
					break;
				} else
				if(capture_isvariablestart(s)){
					IfError_T(expando_variable(buff, &i, what, context), err, { return Error_T(expando_result, err); });
					break;
				} else
				if(capture_istildestart(s)){
					IfError_T(expando_tilde(buff, &i, what, context), err, { return Error_T(expando_result, err); });
					break;
				} else if(strpref("\\$", s) || strpref("\\`", s) || strpref("\\\"", s) || strpref("\\\\", s) || strpref("\\\n", s)){
					buffer_delete(buff, i, i+1);
					i++;
					break;
				} else i++;
			}
		}
	}
	return Error_T(expando_result, {"not a quoted"});
}

ExpandoResult expando_expando(Buffer buff, size_t* si, struct expando_targets what, ParContext context){
	size_t esi = 0, eei, rei;
	if(strpref("$(", str)){
		what.path = true;
		int bal = 1;
		size_t i = (*si)+2;
		while(*s && bal > 0){
			if(capture_isquotstart(s) && !isescaped(s, str)) IfError_T(expando_quot(buff, &i, what, context), err, { return Error_T(expando_result, err); });
			else if(capture_isexpandostart(s) && !isescaped(s, str)) IfError_T(expando_expando(buff, &i, what, context), err, { return Error_T(expando_result, err); });
			else if(capture_isvariablestart(s) && !isescaped(s, str)) IfError_T(expando_variable(buff, &i, what, context), err, { return Error_T(expando_result, err); });
			else if(capture_istildestart(s) && !isescaped(s, str)) IfError_T(expando_tilde(buff, &i, what, context), err, { return Error_T(expando_result, err); });
			else {
				if(!isescaped(s, str)) bal += *s == '(' ? 1 : *s == ')' ? -1 : 0;
				i++;
			}
		}
		if(bal == 0){
			esi = 2;
			eei = (rei=(i-*si))-1;
		}
	}
	if(str[0] == '`'){
		size_t i = *si;
		do {
			string quot = strchr(s+1, '`');
			if(!quot) return Error_T(expando_result, {"couldn't find closing '`'"});
			i = quot-buff->data;
		} while(isescaped(s, str));
		esi = 1;
		eei = (rei=(i-*si)+1)-1;
	}
	if(esi){
		string_mut capt = buffer_destr(buffer_new_from(str+esi, eei-esi));
		if(!capt) return Error_T(expando_result, {"buffer capture failed"});
		PipeResult pipe = pipe_new();
		if(!IsOk_T(pipe)){
			free(capt);
			return Error_T(expando_result, pipe.r.error);
		}
		string_mut captv = null;
		ThreadResult reader = parallels_readstr(pipe.r.ok.read, &captv);
		if(!IsOk_T(reader)) retclean(Error_T(expando_result, {"failed to initialize parallels reader"}), {close(pipe.r.ok.read); close(pipe.r.ok.write);});
		struct parcontext cctxt = *context;
		if(!(cctxt.ios = iosstack_snapdup(cctxt.ios))) retclean(Error_T(expando_result, {"failed to snapshot IO"}), {close(pipe.r.ok.write); thread_waitret(reader.r.ok);});
		iostack_io_open(cctxt.ios, IOSTREAM_STD_OUT, pipe.r.ok.write);
		TihsExeResult captr = tihs_exestr(capt, &cctxt);
		free(capt);
		iosstack_destroy(cctxt.ios);
		ThreadWaitResult captvw = thread_waitret(reader.r.ok);
		if(!IsOk_T(captr)) retclean(Error_T(expando_result, captr.r.error), {free(captv);});
		if(!IsOk_T(captvw)) retclean(Error_T(expando_result, {"failed to wait for reader"}), {free(captv);});
		if(!IsOk(buffer_splice_str(buff, *si, *si+rei, captv))) retclean(Error_T(expando_result, {"captured data splice failed"}), {free(captv);});
		*si += strlen(captv);
		free(captv);
		return Ok_T(expando_result, null);
	}
	return Error_T(expando_result, {"not an expandable"});
}

ExpandoResult expando_variable(Buffer buff, size_t* si, ATTR_UNUSED struct expando_targets what, ParContext context){
	size_t esi = 0, eei, rei;
	if(strpref("${", str)){
		string ent = capture_variable(str);
		if(!ent) return Error_T(expando_result, {"failed to capture variable"});
		esi = 2;
		eei = (rei=ent-str)-1;
	} else if(str[0] == '$'){
		string ent = capture_variable(str);
		if(!ent) return Error_T(expando_result, {"failed to capture variable"});
		esi = 1;
		eei = rei = ent-str;
	}
	if(esi){
		string_mut varn = buffer_destr(buffer_new_from(str+esi, eei-esi));
		if(!varn) return Error_T(expando_result, {"buffer capture failed"});
		struct getvarv varv = parcontext_getunivar(context, varn);
		free(varn);
		if(!varv.v.ref) varv.v.ref = "";
		if(!IsOk(buffer_splice_str(buff, *si, (*si)+rei, varv.v.ref))) retclean(Error_T(expando_result, {"variable value splice failed"}), {if(varv.copy) free(varv.v.copy);});
		*si += strlen(varv.v.ref);
		if(varv.copy) free(varv.v.copy);
		return Ok_T(expando_result, null);
	}
	return Error_T(expando_result, {"not a variable"});
}

ExpandoResult expando_tilde(Buffer buff, size_t* si, ATTR_UNUSED struct expando_targets what, ParContext context){
	if(strpref("~+", str) || strpref("~-", str)){
		struct getvarv varv = parcontext_getunivar(context, strpref("~+", str) ? "PWD" : "OLDPWD");
		if(!varv.v.ref) varv.v.ref = "";
		if(!IsOk(buffer_splice_str(buff, *si, (*si)+2, varv.v.ref))) retclean(Error_T(expando_result, {"variable value splice failed"}), {if(varv.copy) free(varv.v.copy);});
		*si += strlen(varv.v.ref);
		if(varv.copy) free(varv.v.copy);
		return Ok_T(expando_result, null);
	}
	if(strpref("~", str)){
		//TODO user name
		struct getvarv vhome = parcontext_getunivar(context, "HOME");
		Result splok = buffer_splice_str(buff, *si, (*si)+1, vhome.v.ref);
		if(IsOk(splok)) *si += strlen(vhome.v.ref);
		if(vhome.copy) free(vhome.v.copy);
		return IsOk(splok) ? Ok_T(expando_result, null) : Error_T(expando_result, {"variable value splice failed"});;
	}
	return Error_T(expando_result, {"not a tilde expression"});
}

#undef s
#undef str

#define WSEPS "\"'|&;(){}<> \t\r\n"

#define s (buff->data+i)

ExpandoResult expando_word(string str, struct expando_targets what, ParContext context){
	Buffer buff = buffer_new_from(str, -1);
	bool subjectopaexp = what.path;
	size_t i = 0;
	while(buff->data[i]){
		if(capture_isquotstart(s) && !isescaped(s, buff->data)){ IfError_T(expando_quot(buff, &i, what, context), err, { return Error_T(expando_result, err); }); subjectopaexp = false; }
		else if(capture_isexpandostart(s) && !isescaped(s, buff->data)) IfError_T(expando_expando(buff, &i, what, context), err, { return Error_T(expando_result, err); });
		else if(capture_isvariablestart(s) && !isescaped(s, buff->data)) IfError_T(expando_variable(buff, &i, what, context), err, { return Error_T(expando_result, err); });
		else if(capture_istildestart(s) && !isescaped(s, buff->data)){ IfError_T(expando_tilde(buff, &i, what, context), err, { return Error_T(expando_result, err); }); subjectopaexp = false; }
		else if(s[0] == '\\'){
			i++;
			if(!*s){
				buffer_destroy(buff);
				return Error_T(expando_result, {"\\ last character"});
			}
			buffer_delete(buff, i-1, i);
		} else i++;
	}
	if(subjectopaexp){
		if(strchr(buff->data, '*') || strchr(buff->data, '?' || strchr(buff->data, '['))){ //TODO escapes have been deleted above but we may need them for pattern construction
			PatCompResult patco = pattern_compile(context->patcomp, buff->data, *context->parcopts);
			if(IsOk_T(patco)){
				ArgsArr_Mut mf = files_list(patco.r.ok, *context->parcopts);
				pattern_destroy(patco.r.ok);
				if(mf->size > 0 || context->parcopts->nullglob) return Ok_T(expando_result, mf);
				argsarrmut_destroy(mf);
			}
		} else {
			string ifs = varstore_get(context->vars, "IFS");
			if(ifs && *ifs){
				ArgsArr_Mut is = argsarrmut_new(8);
				for(size_t ns = 0; ns < buff->size;){
					for(; ns < buff->size && strchr(ifs, buff->data[ns]); ns++);
					if(ns < buff->size){
						size_t e = ns;
						for(; e < buff->size && !strchr(ifs, buff->data[e]); e++);
						argsarrmut_append(is, strndup(buff->data+ns, e-ns));
						ns = e;
					}
				}
				buffer_destroy(buff);
				return Ok_T(expando_result, is);
			}
		}
	}
	ArgsArr_Mut args = argsarrmut_new(1);
	argsarrmut_append(args, buffer_destr(buff));
	return Ok_T(expando_result, args);
}

#undef s
