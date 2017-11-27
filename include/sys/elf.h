#ifndef _ELF_H
#define _ELF_H
#define PERM_X  0x1 //Execute
#define PERM_W  0x2 //Write
#define PERM_R  0x4 //Read
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/kprintf.h>
void* get_binary(char *filename);
int get_oct_size(char* str);
int load_elf();

#endif