#include <parc24/io.h>
#include <stdio.h>
#include "null.h"
#include "buffer.h"
#include "string.h"
#include <stdarg.h>

static void std_write(LogLevel level, string message, ...){
	#ifdef NDEBUG
	if(level <= LL_DEBUG) return;
	#endif
	if(level >= LL_WARN) fprintf(level >= LL_ERROR ? stderr : stdout, "%s: ", "parc24");
	va_list vargs;
	va_start(vargs, message);
	vfprintf(level >= LL_ERROR ? stderr : stdout, message, vargs);
	va_end(vargs);
	fputc('\n', level >= LL_ERROR ? stderr : stdout);
}

static ParC24IOReadResult std_read(void){
	Buffer line = buffer_new(1024);
	char buff[256] = {'\0'};
	while(true){
		string_mut ok = fgets(buff, sizeof(buff)/sizeof(*buff), stdin);
		if(!ok){
			if(buff[0] == '\0'){ //null for EOF and no characters read
				if(line->size == 0){
					buffer_destroy(line);
					return Ok_T(parc24io_read_result, null);
				} else break;
			}
			buffer_destroy(line);
			return Error_T(parc24io_read_result, {"fgets failed"});
		}
		size_t okl = strlen(ok);
		if(okl == 0) break;
		if(ok[okl-1] == '\n'){
			okl--;
			if(okl > 0 && ok[okl-1] == '\r') okl--;
			buffer_append(line, buff, okl);
			break;
		}
		buffer_append_str(line, buff);
		buff[0] = '\0';
	}
	return Ok_T(parc24io_read_result, buffer_destr(line));
}

ParC24IO parc24io_fromstd(){
	return (struct parc24io){std_write, std_read};
}
