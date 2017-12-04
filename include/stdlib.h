#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

int main();
void exit(int status);


//the memory part
struct super_block{
	size_t size;
	struct super_block* next;
	struct super_block* prev;
	int status;
	int magic_code;
};
void* brk(int pgnum);
void* sbrk(size_t size);
struct super_block* get_free(struct super_block** last, size_t size);
struct super_block* request_block(struct super_block* prev,size_t size);
struct super_block *get_block_header(void *pt);
void free(void *data);
void *malloc(size_t size);




// the file system part
ssize_t write(int fd, const void *buf, size_t count);
#endif
