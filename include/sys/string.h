#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

char *strcpy(char *dest, const char *source);

size_t strlen(char *str);

int strcmp(const char *str1,const char *str2);

char *strstr(const char *haystack, const char *needle);

char *strtok(char *str, const char *delim);

char *strcat(char *dest, const char *src);
#endif
