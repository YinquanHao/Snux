#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H
#include <sys/defs.h>

#define SYS_exit       60
#define SYS_fork       57
#define SYS_getpid     39
#define SYS_getcwd     79
#define SYS_chdir      80
#define SYS_write       1
#define SYS_brk        12
#define SYS_read	0
#define SYS_open        2
#define SYS_close       3
#define SYS_pipe       22
#define SYS_execve     59
#define SYS_mmap	9
#define SYS_getdents   78
#define SYS_munmap     11
#define SYS_wait4      61

void set_msr();
uint64_t syscall_handler();
void syscall_compatible_mode();
void syscall_entry();
void getpid();
uint64_t sys_write();
uint64_t sys_brk();


#endif
