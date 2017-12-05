#include <unistd.h>
#include <syscall.h>

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