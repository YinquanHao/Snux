#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <sys/string.h>


/*pid_t getpid(void) {
	return syscall_0(SYS_getpid);
}

pid_t fork(void){
	//if res==-1
	return syscall_0(SYS_fork);
}*/

ssize_t write(int fd, const void *buf, size_t count) { 
	return syscall_3(SYS_write, fd, (uint64_t)buf, count);
}

void exit(int status){
    syscall_1(SYS_exit, status);
}

void putChar(char c){
	write(1, &c, 1);
}
/*
ssize_t read(int fd, const void *buf, size_t count) { 
	return syscall_3(SYS_read, fd, (uint64_t)buf, count);
}

/*



char *getcwd(char *buf,size_t size){
	return (char *)syscall_2(SYS_getcwd,(uint64_t)buf,size);
}

void *malloc(size_t size){
	void *a=(void*)syscall_6(SYS_mmap,0,size,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,0,0);
	return a;
}

int chdir(char *path){
	//if res==-1
	return syscall_1(SYS_chdir,(uint64_t)path);
}

int pipe(int fildes[2]){
	//if res==-1
	return syscall_1(SYS_pipe,(uint64_t)fildes);
}

int open(const char *pathname, int flags){
	return syscall_2(SYS_open,(uint64_t)pathname,flags);
}

int close(int fd){
	return syscall_1(SYS_close,(uint64_t)fd);
}



void free(void *ptr){
size_t len= sizeof(*ptr);
syscall_2(SYS_munmap,(uint64_t)ptr,len);
}

void *memset(void *s, int c, size_t n){
char* s2 = s;
for(int i=0;i<n;i++){
*s2=c;
s2++;
}
//return s;
}

int execve(const char *filename, char *const argv[], char *const envp[]){
	return syscall_3(SYS_execve,(uint64_t)filename,(uint64_t)argv,(uint64_t)envp);
}

pid_t waitpid(pid_t pid, int *status, int options){
	pid_t res= syscall_3(SYS_wait4,(uint64_t)pid,(uint64_t)status,(uint64_t)options);
	return res;
}


struct dir* opendir(const char *name){

int fd=open(name, O_DIRECTORY);	
struct dir *res=malloc(sizeof(struct dir));
char buf[1024];
int bpos;
res->nread=syscall_3(SYS_getdents,fd,(uint64_t)buf,(uint64_t)sizeof(struct dirent));
	res->fd=fd;
	res->data=buf;
	strcpy(res->dirname,name);
//printf("dirname:%s\n",res->dirname);
	//printf("nread:%d\n",res->nread);
struct dirent *dt=(struct dirent *)res->data;
for(bpos=0;bpos<res->nread;){
	dt=(struct dirent *)(res->data+bpos);
	//printf("%s\n",dt->d_name);
	bpos=bpos+dt->d_reclen;
}
	//d=(struct dirent *)buf;
	//printf("%s",d->d_name);
	return res;
}

void readdir(struct dir *d){
int bpos;
struct dirent *dt=(struct dirent *)d->data;	
//printf("%d\n",d->nread);
for(bpos=0;bpos<d->nread;){
	dt=(struct dirent *)(d->data+bpos);
	printf("%s\n",dt->d_name);
	bpos=bpos+dt->d_reclen;
}
}

int closedir(struct dir *d){
struct dir *dptr=(struct dir*)d;
if(dptr!=NULL){
	close(dptr->fd);
	free(dptr);
}
return -1;
}*/










