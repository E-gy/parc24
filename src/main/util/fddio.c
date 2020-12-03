#include "fddio.h"

#include <unistd.h>
#include "string.h"
#include "buffer.h"

Result fddio_writestr(fd_t f, string str){
	if(f < 0 || !str) return Error;
	const size_t strl = strlen(str);
	size_t ww = 0;
	while(ww < strl){
		int w = write(f, str+ww, strl-ww);
		if(w <= 0) return Error;
		ww += w;
	}
	return Ok;
}

Result fddio_readstr(fd_t f, string_mut* str){
	if(f < 0 || !str) return Error;
	Buffer s = buffer_new(0);
	if(!s) return Error;
	char buff[4096] = {'\0'};
	while(true){
		int r = read(f, buff, sizeof(buff));
		if(r < 0) return Error;
		if(r == 0) break;
		if(!IsOk(buffer_append(s, buff, r))){
			buffer_destroy(s);
			return Error;
		}
	}
	*str = buffer_destr(s);
	return Ok;
}
