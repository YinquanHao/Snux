#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H
#include <sys/defs.h>
#include <dirent.h>

#define SYS_exit       60
#define SYS_fork       57
#define SYS_getpid     39
#define SYS_getcwd     79
#define SYS_chdir      80
#define SYS_write       1
#define SYS_brk        12
#define SYS_read		0
#define SYS_open        2
#define SYS_close       3
#define SYS_pipe       22
#define SYS_execve     59
#define SYS_mmap		9
#define SYS_getdents   78
#define SYS_munmap     11
#define SYS_wait4      61
#define SYS_opendir     4
#define SYS_readdir     5
#define SYS_closedir    6
#define O_RDONLY  		0
#define O_WRONLY		1

void set_msr();
uint64_t syscall_handler();
void syscall_compatible_mode();
void syscall_entry();
pid_t getpid();
uint64_t sys_write();
uint64_t sys_brk();
int sys_closedir(struct DIR* dir);
dirent* sys_readdir(struct DIR* dir);
DIR* sys_opendir(char *path);
int sys_open(char *path, uint64_t flag);
int sys_read(uint64_t fd_count,uint64_t addr,uint64_t len);
int sys_close(int fd);
uint64_t sys_getdents(int fd,uint64_t buf,uint64_t len);
uint64_t sys_getcwd(char *buf, size_t size);
int sys_chdir(char *path);
#endif
