#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

static __inline uint64_t syscall_0(uint64_t syscallNum) {
	uint64_t ret;
    __asm__ __volatile__(
            "movq %1,%%rax;"
            "syscall;"
            "movq %%rax,%0;"
            :"=a"(ret)
            :"a"(syscallNum)
            : "memory");
    return ret;
}

static __inline uint64_t syscall_1(uint64_t syscallNum, uint64_t a1) {
	uint64_t ret;
    __asm__ __volatile__(
            "movq %1,%%rax;"
            "movq %2,%%rdi;"
            "syscall;"
            "movq %%rax,%0;"
            :"=a"(ret)
            :"a"(syscallNum),"D"(a1)
            : "memory");
    //kprintf("syscall_1 ret %x",ret);
    return ret;
}

static __inline uint64_t syscall_2(uint64_t syscallNum, uint64_t a1, uint64_t a2) {
    uint64_t ret;
    __asm__ __volatile__(
            "movq %1,%%rax;"
            "movq %2,%%rdi;"
            "movq %3,%%rsi;"
            "syscall;"
            "movq %%rax,%0;"
            :"=a"(ret)
            :"a"(syscallNum),"D"(a1),"S"(a2)
            : "memory");
    return ret;
}


static __inline uint64_t syscall_3(uint64_t syscallNum, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t ret;
    __asm__ __volatile__(
            "movq %1,%%rax;"
            "movq %2,%%rdi;"
            "movq %3,%%rsi;"
            "movq %4,%%rdx;"
            "syscall;"
            "movq %%rax,%0;"
            :"=a"(ret)
            :"a"(syscallNum), "D"(a1), "S"(a2), "d"(a3)
            : "memory");
    return ret;
}

static __inline uint64_t syscall_6(uint64_t syscallNum,uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5,uint64_t a6){
    uint64_t ret;
    __asm__ __volatile__(
            "movq %1,%%rax;"
            "movq %2,%%rdi;"
            "movq %3,%%rsi;"
            "movq %4,%%rdx;"
            "movq %5,%%r10;"
            "movq %6,%%r8;"
            "movq %7,%%r9;"
            "syscall;"
            "movq %%rax,%0;"
            :"=a"(ret)
            :"a"(syscallNum),"D"(a1),"S"(a2),"d"(a3),"r"(a4),"r"(a5),"r"(a6)
            :"memory");
    return ret;
}

#endif

