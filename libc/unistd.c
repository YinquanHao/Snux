#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

int open( const char *pathname, int flags){
	int res = (int)syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);
	return res;
}


ssize_t read(int fd, void *buf, size_t count) {
	if(fd < 0){
		return 0;
	}
	ssize_t res= (ssize_t)syscall_3(SYS_read,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
	if(res<1){
		return -1;
	}
	return res;
}

int close(int fd){
	int res = (int)syscall_1(SYS_close,fd);
	return res;
}


int chdir(const char *path){
	int res = (int)syscall_1(SYS_chdir,path);
	return res;
}

char *getcwd(char *buf, size_t size){
	uint64_t res = (uint64_t)syscall_2(SYS_getcwd,buf,size);
	return res;
}


int execvpe( const char *filename, char *const argv[], char *const envp[]){        
	if(filename==NULL){
		return -1;
	}
	int res = (int)syscall_3(SYS_execve,(uint64_t)filename,(uint64_t)argv,(uint64_t)envp);
	return res;
}

pid_t fork(void){
	//if res==-1
	uint64_t res =  syscall_0(SYS_fork);
	printf("res %d\n", res);
	return res;
}

int waitpid(int pid, int *status){
	return syscall_3(SYS_wait4,pid,status,0);
}