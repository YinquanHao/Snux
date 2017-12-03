#ifndef __KPRINTF_H
#define __KPRINTF_H

void kprintf(const char *fmt, ...);
void kprintt(const char *fmt,...);
void kprintkb(char state,char key);
void scroll();
void move_line(int dest,int source);
#endif
