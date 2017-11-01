#include <sys/virtualmem.h>
#include <sys/physmem.h>
#include <sys/kprintf.h>

pml4_t PML4;
pml4_t PML41;

void init_kernalmem(unsigned long physfree){
	//kernal_base is the virtual address where actual kernal ends. The address is the start point we can used for kmalloc
	unsigned long kernal_base = (physfree + PG_DESC_SIZE*PAGE_SIZE) + VIRT_ST;
    /*
    kprintf("print physfree !!!! %x \n",physfree);
    kprintf("print kernal_base !!!! %x \n",kernal_base);
    kprintf("print total length %x \n",total);
    */
	memset((void *) kernal_base, 0,PAGE_SIZE*128);
	PML4 = (pml4_t)kmalloc(KERNAL_TB,PAGE_SIZE);
    //PML41 = (pml4_t)kmalloc(KERNAL_TB,PAGE_SIZE);
    kprintf("address of PML4 %x", PML4);
    //kprintf("address of PML41 %x", PML41);


}

void* kmalloc(int flag, unsigned int size){
    unsigned long res_addr = 0x0;
    switch(flag){
        case KERNAL_TB:
            res_addr = allocate_page();
            break;
        default:
            res_addr = 0x0;
    }
    kprintf("kmalloc address returned %x \n", res_addr);
    return (void*)res_addr;
}
/*
void phys_to_virtu_map(int is_kernal, unsigned long size){
    unsigned long virt_addr = VIRT_ST;
    unsigned long phy_addr = 0;
    while (phy_addr < size) {
        if (is_kernal == KERN_MAP) {
            vir_phy_mapping(vir_addr, phy_addr, KERNPT, KERN_MAP,
            NEED_SET_PTE_FLAGS);
        } else {
            vir_phy_mapping(vir_addr, phy_addr, USERPT, KERN_MAP,
            NEED_SET_PTE_FLAGS);
        }
        phy_addr += PAGE_SIZE;
        virt_addr += PAGE_SIZE;
    }
}


void vir_phy_mapping(unsigned long vir_addr, unsigned long phy_addr){
    pdpt_t pdpt_tab;
    pdt_t pdt_tab;
    pt_t pt_tab;

    unsigned long pml4_index = *(unsigned long*)vir_addr >> 39 & 0x1FF;
    printf("pml4_index %lu\n", pml4_index);
    unsigned long page_dir_pt_index  =  *(unsigned long*)vir_addr >> 30 & 0x1FF;
    printf("page_dir_pt_index %lu\n", page_dir_pt_index);
    unsigned long page_dir_index  = *(unsigned long*)vir_addr >> 21 & 0x1FF;
    printf("page_dir_index %lu\n", page_dir_index);
    unsigned long page_tb_index =  *(unsigned long*)vir_addr >> 12 & 0x1FF;
    printf("page_tb_index %lu\n", page_tb_index);
}





void* set_pdpt(pml4_t pml4, unsigned long pml4_index, int kmalloc_flag,
        int user_or_kern_map) {
    pdpt_t pdpt = (pdpt_t) kmalloc(kmalloc_flag,1);
    uint64_t pdpt_entry = (uint64_t) pdpt;
    pdpt_entry -= VIR_START; // convert to physical address

    if (user_or_kern_map == USER_MAP) {
        pdpt_entry |= PTE_U;
    }

    pdpt_entry |= (PTE_P | PTE_W);

    pml4->PML4E[pml4_index] = pdpt_entry;

    return (void *) pdpt;
}
*/

void *memset(void *s, int ch , unsigned long n) {
    char* tmp = s;
    while(n > 0) {
        *tmp = ch;
        tmp++;
        n--;
    }
    return s;
}




