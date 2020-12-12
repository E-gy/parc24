#include <ctype.h>

int char_isword(int c){
	return c == '_' || isalnum(c);
}
