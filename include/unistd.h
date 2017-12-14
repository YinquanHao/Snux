#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>
#include <sys/syscall.h>
//Done
int open(const char *pathname, int flags);
//Done
int close(int fd);
//Done
ssize_t read(int fd, void *buf, size_t count);
//compltet??
ssize_t write(int fd, const void *buf, size_t count);

int unlink(const char *pathname);
//Done
int chdir(const char *path);
//Done
char *getcwd(char *buf, size_t size);
//Done
pid_t fork();
//Done
int execvpe(const char *file, char *const argv[], char *const envp[]);

pid_t wait(int *status);
//Done
int waitpid(int pid, int *status);

unsigned int sleep(unsigned int seconds);

pid_t getpid(void);

pid_t getppid(void);
//Done
int listfiles(char *path);
// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
//int mkdir(const char *pathname, mode_t mode);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);
void clear_screen();
void ps();
int cat(char *path);
unsigned int sleep(unsigned int seconds);

#endif
