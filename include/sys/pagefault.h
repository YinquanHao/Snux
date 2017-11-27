#ifndef _PAGEFAULT_H
#define _PAGEFAULT_H
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/kprintf.h>
#include <sys/process.h>
#include <sys/physmem.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>

void page_fault_handler(struct regs *r);
void demand_paging(uint64_t faddr);

#endif
