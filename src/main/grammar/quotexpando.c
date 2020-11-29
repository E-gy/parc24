#include "quotexpando.h"

#include <util/string.h>
#include <util/null.h>
#include <util/buffer.h>

string_mut expando_word(string str, struct expando_targets what, ParContext context){
	Buffer buff = buffer_new_from(str, -1);
	//TODO
	return buffer_destr(buff);
}
