#include "bufferio.h"

#include <stdio.h>

BufferResult buffer_from_file(string file){
	if(!file) return Error_T(buffer_from_result, {"No file specified"});
	FILE* f = fopen(file, "r");
	if(!f) return Error_T(buffer_from_result, {"Failed to read specified file"});
	Buffer b = buffer_new(4096);
	if(!b){
		fclose(f);
		return Error_T(buffer_from_result, {"Buffer allocation error"});
	}
	char buff[4096];
	while(true){
		size_t r = fread(buff, sizeof(*buff), 4096, f);
		buffer_append(b, buff, r);
		if(r != 4096) break;
	}
	fclose(f);
	return Ok_T(buffer_from_result, b);
}
