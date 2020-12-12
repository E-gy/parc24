#include "string.h"

#include <calp/util/buffer.h>
#include "null.h"

string_mut strdup(string src){
	// if(!src) return null;
	return buffer_destr(buffer_new_from(src, -1));
}

string_mut strndup(string src, size_t n){
	// if(!src) return null;
	const size_t srclen = strlen(src);
	return buffer_destr(buffer_new_from(src, srclen < n ? srclen : n));
}
