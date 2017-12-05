#include <dirent.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <sys/virtualmem.h>
#include <stdlib.h>


int closedir(DIR *dirp){
    int ret = close(dirp->fd);
	free(dirp);
	return ret;
}


DIR *opendir(const char *name){
	int fd = open(name, O_RDONLY);
	DIR *dir;
	if(fd<0) return NULL;
	if (!(dir = malloc(sizeof(*dir)))) {
		syscall_1(SYS_close, fd);
		return 0;
	}
	dir->fd = fd;
	return dir;
}



struct dirent *readdir(DIR *dir){
	struct dirent *de;
	int len = syscall_3(SYS_getdents, dir->fd, dir->buf, sizeof(dir->buf));
	if (len <= 0) {
		return NULL;
	}
	de = (void *)(dir->buf);
	return de;
}
