#include <parc24/ioslog.h>
#include <parc24/iosstack.h>
#include <stdio.h>
#include <util/null.h>
#include <util/buffer_printf.h>
#include <util/fddio.h>
#include <util/string.h>

IOsStack parcio_new_fromstd(void){
	IOsStack ios = iosstack_new();
	if(!ios) return null;
	iosstack_raw_set(ios, STDIN_FILENO, STDIN_FILENO);
	iosstack_raw_set(ios, STDOUT_FILENO, STDOUT_FILENO);
	iosstack_raw_set(ios, STDERR_FILENO, STDERR_FILENO);
	return ios;
}

void parciolog(IOsStack io, enum log_level level, string str, ...){
	Buffer buff = buffer_new(128);
	if(!buff) return;
	if(level >= LL_WARN && !IsOk(buffer_printf(buff, "%s: ", "parc24"))){ buffer_destroy(buff); return; }
	va_list args;
	va_start(args, str);
	Result prr = buffer_vprintf(buff, str, args);
	va_end(args);
	if(!IsOk(prr)){ buffer_destroy(buff); return; }
	if(!IsOk(buffer_append_str(buff, "\n"))){ buffer_destroy(buff); return; }
	fddio_writestr(iosstack_raw_get(io, level >= LL_ERROR ? STDERR_FILENO : STDOUT_FILENO), buff->data);
	buffer_destroy(buff);
}

void parcioprintf(IOsStack io, enum log_level level, string str, ...){
	Buffer buff = buffer_new(128);
	if(!buff) return;
	va_list args;
	va_start(args, str);
	Result prr = buffer_vprintf(buff, str, args);
	va_end(args);
	if(!IsOk(prr)){ buffer_destroy(buff); return; }
	fddio_writestr(iosstack_raw_get(io, level >= LL_ERROR ? STDERR_FILENO : STDOUT_FILENO), buff->data);
	buffer_destroy(buff);
}

ParC24IOReadResult parcioread(IOsStack io){
	string_mut str;
	if(!IsOk(fddio_readstr(iosstack_raw_get(io, STDIN_FILENO), &str))) return Error_T(parc24io_read_result, {"failed to read"});
	return Ok_T(parc24io_read_result, str);
}

/*static void std_write(LogLevel level, string message, ...){
	#ifdef NDEBUG
	if(level <= LL_DEBUG) return;
	#endif
	if(level >= LL_WARN) fprintf(level >= LL_ERROR ? stderr : stdout, "%s: ", "parc24");
	va_list vargs;
	va_start(vargs, message);
	vfprintf(level >= LL_ERROR ? stderr : stdout, message, vargs);
	va_end(vargs);
	fputc('\n', level >= LL_ERROR ? stderr : stdout);
}*/

ParC24IOReadResult parcio_derp_std_read(void){
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
