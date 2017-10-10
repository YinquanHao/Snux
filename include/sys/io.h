
#ifndef _IO_H
#define _IO_H

#include<sys/defs.h>
#include<sys/kprintf.h>
static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ __volatile__ ( "outb %0, %1" 
    					 : : "a"(value), "Nd"(port) );
}

static inline unsigned char inb(unsigned short port){
    unsigned char res;
    __asm__ __volatile__( "inb %1, %0"
                         : "=a"(res) : "Nd"(port) );
    return res;
}

static __inline unsigned int inl2(unsigned short int port)
{
   unsigned int _result;
   //__asm__ __volatile__("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
__asm__ __volatile__ ("inl %w1, %0" : "=a" (_result) : "Nd" (port));
  return _result;

}

static __inline void outl2(unsigned short int port, unsigned int data)
{
   //__asm__ __volatile__("outl %%eax, %%dx" : : "Nd" (port), "a" (data));
__asm__ __volatile__ ("outl %0, %w1" : : "a"(data), "Nd"(port));

}


static __inline void outl(uint16_t port, uint32_t value){
   __asm__ __volatile__("outl %1, %0" : : "Nd" (port), "a" (value));
}

static __inline uint32_t inl(uint16_t port){
   uint32_t ret;
   __asm__ __volatile__("inl %1, %0" : "=a" (ret) : "Nd" (port));
   return ret;
}
#endif
