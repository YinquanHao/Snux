#ifndef _ELF_H
#define _ELF_H
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/kprintf.h>
void* get_binary(char *filename);
size_t get_oct_size(char* str);
int load_elf();

#endif