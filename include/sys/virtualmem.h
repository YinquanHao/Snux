#ifndef _VIRTUALMEM_H
#define _VIRTUALMEM_H

#define VIRT_ST 0xFFFFFFFF80000000UL
#define PAGE_SIZE 0x1000 //4KB for each PAGE_SIZE
#define KERNAL_PG_TB_NUM 128 //the maximum number of pages for kernal	
#define PG_OCCU 1
#define TABLE_SIZE 512
#define KERNAL_TB 0

struct pml4t {
    unsigned long  PML4E[TABLE_SIZE];
};
typedef struct pml4t* pml4_t;

struct pdpt {
	unsigned long PDPTE[TABLE_SIZE];
};
typedef struct pdpt* pdpt_t;

struct pdt {
	unsigned long PDTE[TABLE_SIZE];
};
typedef struct pdt* pdt_t;

struct pt {
	unsigned long PTE[TABLE_SIZE];
};
typedef struct pt* pt_t;



void init_kernalmem(unsigned long physfree);
void *memset(void *s, int ch , unsigned long n);
void* kmalloc(int flag, unsigned int size);

#endif
