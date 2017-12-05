#include <dirent.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <sys/virtualmem.h>
#include <stdlib.h>

DIR *opendir(const char *name){
	DIR* dirt = (DIR *)syscall_1(SYS_opendir,(uint64_t)name);
	return dirt;
}

struct dirent *readdir(DIR *dirp){
	dirent* res;
	res = (dirent *)syscall_1(SYS_readdir,(uint64_t)dirp);
	return res;	
}

int closedir(DIR *dirp){
    int res = (int)syscall_1(SYS_closedir,(uint64_t)dirp);
}