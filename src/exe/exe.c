#include <lib.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* args[]){
	char* str = sayhi(argc > 1 ? args[1] : NULL);
	if(!str) return 1;
	puts(str);
	free(str);
	return 0;
}
