#include "buffer_printf.h"

#include <stdio.h>

Result buffer_printf(Buffer buff, string fmt,...){
	va_list args;
	va_start(args, fmt);
	Result r = buffer_vprintf(buff, fmt, args);
	va_end(args);
	return r;
}

Result buffer_vprintf(Buffer buff, string fmt, va_list args){
	const size_t useable = buff->cap - buff->size;
	va_list argscpy;
	va_copy(argscpy, args);
	const ssize_t tsrr = vsnprintf(buff->data+buff->size, useable, fmt, args);
	if(tsrr < 0) return Error;
	const size_t tsr = tsrr;
	if(tsr < useable){
		buff->data[buff->size += tsr] = '\0';
		return Ok;
	}
	if(!IsOk(buffer_resize(buff, buff->size + tsr))) return Error;
	return buffer_vprintf(buff, fmt, argscpy);
}
