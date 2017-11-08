#ifndef _VIRTUALMEM_H
#define _VIRTUALMEM_H

#define VIRT_ST 0xFFFFFFFF80000000UL
#define PAGE_SIZE 0x1000 //4KB for each PAGE_SIZE
#define KERNAL_PG_TB_NUM 128 //the maximum number of pages for kernal	
#define PG_OCCU 1
#define TABLE_SIZE 512
#define KERNAL_TB 0
#define PG_DESC_SIZE 256
#define PML4_OFFSET 39
#define PDPT_OFFSET 30
#define PDT_OFFSET  21
#define PT_OFFSET   12
#define KERNAL_MALLOC 1
#define USER_MALLOC 0
#define PERM_MASK 0xFFFFFFFFFFFFF000 //mask the permission bits

#define PT_P 0x001 //present
#define PT_W 0x002 //writable
#define PT_U 0x004 //user

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
void vir_phy_mapping(unsigned long vir_addr, unsigned long phy_addr);
void* set_pt(pdt_t pdt, unsigned long pdt_index, int is_kernal);
void* set_pdt(pdpt_t pdpt, unsigned long pdpt_index, int is_kernal);
void* set_pdpt(pml4_t pml4, unsigned long pml4_index, int is_kernal);
unsigned long get_vir_from_phy(unsigned long phys_addr);
void map_kernel(unsigned long map_size);
void init_virt_phy_mapping();
//void set_CR3(unsigned long CR3);
void set_CR3(pml4_t new_cr3);
#endif
