#include "flist.h"

#include "null.h"
#include "string.h"
#include <dirent.h>
#include "buffer.h"
#include <ctype.h>

static Result files_list_(ArgsArr_Mut files, string dir, Pattern pat, struct parc_options opts){
	if(!streq(".", dir)){
		string dirnopref = strpref("./", dir) ? dir+2 : dir;
		bool match;
		if(opts.nocaseglob){
			Buffer buff = buffer_new_from(dirnopref, -1);
			if(!buff) return Error;
			for(size_t i = 0; i < buff->size; i++) if(isupper(buff->data[i])) buff->data[i] += 'a' - 'A';
			match = pattern_test(pat, buff->data);
			buffer_destroy(buff);
		} else match = pattern_test(pat, dirnopref);
		if(match && !IsOk(argsarrmut_append(files, strdup(dirnopref)))) return Error;
	}
	DIR* d = opendir(dir);
	if(!d) return Ok;
	struct dirent* e;
	while((e = readdir(d))) if(!streq(".", e->d_name) && !streq("..", e->d_name) && (opts.dotglob || e->d_name[0] != '.')){
		Buffer buff = buffer_new_from(dir, -1);
		if(!buff || !IsOk(buffer_append_str(buff, "/")) || !IsOk(buffer_append_str(buff, e->d_name))) retclean(Error, { buffer_destroy(buff); });
		if(!IsOk(files_list_(files, buff->data, pat, opts))) retclean(Error, { buffer_destroy(buff); });
		buffer_destroy(buff);
	}
	closedir(d);
	return Ok;
}

ArgsArr_Mut files_list(Pattern pattern, struct parc_options opts){
	ArgsArr_Mut files = argsarrmut_new(16);
	if(!files) return null;
	if(!IsOk(files_list_(files, ".", pattern, opts))){
		argsarrmut_destroy(files);
		return null;
	}
	return files;
}
