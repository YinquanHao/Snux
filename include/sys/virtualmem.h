#ifndef _VIRTUALMEM_H
#define _VIRTUALMEM_H
#include <sys/defs.h>

#define VIRT_ST 0xFFFFFFFF80000000UL
#define PAGE_SIZE 0x1000 //4KB for each PAGE_SIZE
#define KERNAL_PG_TB_NUM 128 //the maximum number of pages for kernal	
#define PG_OCCU 1
#define TABLE_SIZE 512
#define KERNAL_TB 0
#define KERNAL_MEM 1
#define USER_MEM 2
#define PG_DESC_SIZE 256
#define PML4_OFFSET 39
#define PDPT_OFFSET 30
#define PDT_OFFSET  21
#define PT_OFFSET   12
#define KERNAL_MALLOC 1
#define USER_MALLOC 2
#define PERM_MASK 0xFFFFFFFFFFFFF000 //mask the permission bits

#define PT_P 0x001 //present
#define PT_W 0x002 //writable
#define PT_U 0x004 //user
#define PT_COW 0x800 //copy on write 1000-0000-0000

#define PML4_LEVEL 1
#define PDPT_LEVEL 2
#define PDT_LEVEL 3
#define PT_LEVEL 4



#define PML4_SELF 0xFFFFFF7FBFDFE000
#define PDPT_SELF 0xFFFFFF7FBFC00000
#define PDT_SELF  0xFFFFFF7F80000000
#define PT_SELF   0xFFFFFF0000000000
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
void init_virt_phy_mapping(unsigned long mem_length);
void mapping_test();
void set_CR3(pml4_t new_cr3);
uint64_t get_CR3();
void* set_user_addr_space();
uint64_t get_physical_addr(uint64_t vaddr);
void user_process_mapping(uint64_t vir_addr, uint64_t phy_addr, pml4_t user_PML4,uint64_t flags);
void user_process_mapping_v2(uint64_t vir_addr, uint64_t phy_addr, pml4_t user_PML4,uint64_t flags);
uint64_t allocate_vir_page(uint64_t vaddr,uint64_t flags);
void *memcpy(void *dest,const void *src,uint64_t n);
void test_self_ref();

//uint64_t get_tb_virtual_addr(uint64_t level, uint64_t entry_belong_to);
uint64_t get_tb_virt_addr(uint64_t level, uint64_t virt_addr);
uint64_t get_physical_addr_user(uint64_t virt_addr);

void set_pml4e_bits(uint64_t vaddr, uint64_t flags);


void set_pdpte_bits(uint64_t vaddr, uint64_t flags);

void set_pdte_bits(uint64_t vaddr, uint64_t flags);


void set_pte_bits(uint64_t vaddr, uint64_t flags);
#endif
