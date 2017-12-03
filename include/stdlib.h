#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

int main();
void exit(int status);

void *malloc(size_t size);
void free(void *ptr);
ssize_t write(int fd, const void *buf, size_t count);
#endif
