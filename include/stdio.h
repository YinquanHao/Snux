#ifndef _STDIO_H
#define _STDIO_H

static const int EOF = -1;

int putchar(int c); //int?
int puts(const char *s); //tested
int printf(const char *format, ...);

char *gets(char *s); //tested
void *memset(void *s, int ch , unsigned long n);
unsigned long toInt(const char *a);
#endif
