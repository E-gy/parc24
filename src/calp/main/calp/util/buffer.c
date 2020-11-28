#include <calp/util/buffer.h>

#include <stdlib.h>
#include "null.h"
#include "string.h"
#include "math.h"

Buffer buffer_new(size_t inicap){
	char* data = calloc(inicap+1, sizeof(*data));
	if(!data) return null;
	Buffer buff = malloc(sizeof(*buff));
	if(!buff){
		free(data);
		return null;
	}
	*buff = (struct buffer){ inicap, 0, data };
	return buff;
}

string_mut buffer_destr(Buffer b){
	if(!b) return null;
	string_mut str = b->data;
	free(b);
	return str;
}

void buffer_destroy(Buffer b){
	free(buffer_destr(b));
}

Buffer buffer_new_from(string str, ssize_t len){
	const size_t lenpo = len;
	const size_t l = len < 0 ? str ? strlen(str) : 0 : lenpo;
	Buffer b = buffer_new(l);
	if(!b) return null;
	memcpy(b->data, str, b->size = l);
	return b;
}

Result buffer_resize(Buffer b, size_t newcap){
	if(!b) return Error;
	if(newcap == b->cap) return Ok;
	char* nd = realloc(b->data, newcap+1);
	if(!nd) return Error;
	nd[newcap] = '\0';
	*b = (struct buffer){newcap, min(newcap, b->size), nd};
	return Ok;
}

Result buffer_splice(Buffer b, size_t from, size_t toe, string d, ssize_t len){
	if(!b) return Error;
	if((toe = min(b->size, toe)) < (from = min(b->size, from))) return Error;
	const size_t lenpo = len;
	const size_t il = len < 0 ? d ? strlen(d) : 0 : lenpo;
	const size_t dl = toe-from;
	const size_t ns = b->size - dl + il;
	if(ns > b->cap) if(buffer_resize(b, ns) != Ok) return Error;
	memmove(b->data+from+il, b->data+toe, b->size-toe+1); //+1 for delim
	if(il) memcpy(b->data+from, d, il);
	b->size = ns;
	return Ok;
}