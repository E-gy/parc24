#include <cppo.h>

#include <stdlib.h>
#include <util/null.h>

#ifdef _WIN32

#include <errhandlingapi.h>

/*
#include <winnt.h>

// Hackity hackity
// #include <winbase.h>
#ifndef FORMAT_MESSAGE_ALLOCATE_BUFFER
#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#endif // FORMAT_MESSAGE_ALLOCATE_BUFFER
#ifndef FORMAT_MESSAGE_IGNORE_INSERTS
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#endif // FORMAT_MESSAGE_IGNORE_INSERTS
#ifndef FORMAT_MESSAGE_FROM_SYSTEM
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#endif // FORMAT_MESSAGE_FROM_SYSTEM

DWORD FormatMessage(DWORD dwFlags, void* lpSource, DWORD dwMessageId, DWORD dwLanguageId, string_mut* lpBuffer, DWORD nSize, va_list *Arguments);

string_mut get_last_error_desc(void){
    string_mut str;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &str, 0, NULL );
	return str;
}*/

#include <stdio.h>

void exe_print_last_syserr(string addinfo){
	fprintf(stderr, "%s%sWindows Error 0x%lx\n", addinfo ? addinfo : "", addinfo ? ": " : "", GetLastError());
}

#else

#include <stdio.h>

void exe_print_last_syserr(string addinfo){
	perror(addinfo);
}

#endif
