#include "fdi.h"

#include <unistd.h>
#include <fcntl.h>

#ifdef _WIN32

static Result fdgetflags(ATTR_UNUSED fd_t fd, int* flags){
	*flags = O_RDONLY&O_WRONLY;
	return Ok; //FIXME eh:/
}

#else

static Result fdgetflags(fd_t fd, int* flags){
	return (*flags = fcntl(fd, F_GETFL)) < 0 ? Error : Ok;
}

#endif

bool fdisreadable(fd_t fd){
	int flags;
	return IsOk(fdgetflags(fd,&flags)) && (flags&O_RDONLY) == O_RDONLY;
}

bool fdiswriteable(fd_t fd){
	int flags;
	return IsOk(fdgetflags(fd,&flags)) && (flags&O_WRONLY) == O_WRONLY;
}

bool fdisreadwriteable(fd_t fd){
	int flags;
	return IsOk(fdgetflags(fd,&flags)) && (flags&O_RDWR) == O_RDWR;
}
