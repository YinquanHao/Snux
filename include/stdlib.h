#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>
//TODO yinquanhao enable the arguments
int main(int argc, char* argv[], char* envp[]);

//Done
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
//Done
void free(void *data);
//Done
void *malloc(size_t size);


char *strcpy(char *dest, const char *source);

size_t strlen(char *str);

int strcmp(const char *str1,const char *str2);

char *strstr(const char *haystack, const char *needle);

char* strtok(char *str, const char* delim);

char *strcat(char *dest, const char *src);
int printchar(char c);


//Done?
ssize_t write(int fd, const void *buf, size_t count);
#endif
