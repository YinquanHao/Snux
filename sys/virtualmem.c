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
    kprintf("address of PML4 %x \n", PML4);
    //kprintf("address of PML41 %x", PML41);
}


void init_virt_phy_mapping() {
    map_kernel(0x2000000);//test for 32M mapping 
    //set_CR3((unsigned long) PML4-VIRT_ST);
}


void map_kernel(unsigned long map_size){
    unsigned long vir_st  = VIRT_ST;
    unsigned long phy_st = 0;
    //while(phy_st<map_size){
    int k=0;
    while(k<1000){
        vir_phy_mapping(vir_st,phy_st);
        vir_st += PAGE_SIZE;
        phy_st += PAGE_SIZE;
        k++;
    }
    ///}
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
    memset(res_addr,0,4096);
    return (void*)res_addr;
}

void vir_phy_mapping(unsigned long vir_addr, unsigned long phy_addr){
    pdpt_t pdpt_tab;
    pdt_t pdt_tab;
    pt_t pt_tab;
    unsigned long entry_address;
    unsigned long pml4_index = *(unsigned long*)vir_addr >> 39 & 0x1FF;
    //printf("pml4_index %lu\n", pml4_index);
    unsigned long page_dir_pt_index  =  *(unsigned long*)vir_addr >> 30 & 0x1FF;
    //printf("page_dir_pt_index %lu\n", page_dir_pt_index);
    unsigned long page_dir_index  = *(unsigned long*)vir_addr >> 21 & 0x1FF;
    //printf("page_dir_index %lu\n", page_dir_index);
    unsigned long page_tb_index =  *(unsigned long*)vir_addr >> 12 & 0x1FF;
    //printf("page_tb_index %lu\n", page_tb_index);
    unsigned long pml4_entry=PML4->PML4E[pml4_index];

    kprintf("pml4_index %d\n",pml4_index);
    kprintf("pml4_entry %x\n",pml4_entry);
    
    if(pml4_entry & PT_P ){                        //if the given entry presented in the table, use it
        entry_address=pml4_entry&PERM_MASK;    //mask the permission bits of the address  
        pdpt_tab=(pdpt_t)(entry_address);
    }else{
        //kprintf("enfie \n");                                          //set up new pdpt for entry
        pdpt_tab=(pdpt_t)set_pdpt(PML4,pml4_index,KERNAL_MALLOC);
    }

    unsigned long pdpt_entry=pdpt_tab->PDPTE[page_dir_pt_index];
    if(pdpt_entry & PT_P ){ 
        entry_address=(pdpt_entry)&PERM_MASK;      
        pdt_tab=(pdt_t)(entry_address);
    }else{
        pdt_tab=(pdt_t)set_pdt(pdpt_tab,page_dir_pt_index,KERNAL_MALLOC);
    }
    unsigned long pdt_entry=pdt_tab->PDTE[page_dir_index];
    if(pdt_entry & PT_P ){ 
        entry_address=(pdt_entry)&PERM_MASK;      
        pt_tab=(pt_t)(entry_address);
    }else{
        pt_tab=(pt_t)set_pt(pdt_tab,page_dir_index,KERNAL_MALLOC);
        //kprintf("enter else1 \n");
    }
    unsigned long page_entry=phy_addr;
    page_entry=page_entry|PT_P|PT_U|PT_W; //set the page table flags
    pt_tab->PTE[page_tb_index]=page_entry;
}


/*get virtual address from given physical address*/
unsigned long get_vir_from_phy(unsigned long phys_addr){
    return (phys_addr|VIRT_ST);
}


void* set_pdpt(pml4_t pml4, unsigned long pml4_index, int is_kernal) {
    pdpt_t pdpt = (pdpt_t) kmalloc(KERNAL_TB,1);
    unsigned long pdpt_entry = (unsigned long) pdpt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pdpt_entry |= PT_U;
    }
    //set writtable and present bits
    pdpt_entry |= (PT_P | PT_W);
    /*pdpt_entry is physical address*/
    pml4->PML4E[pml4_index] = pdpt_entry;
    return (void *) pdpt;
}


void* set_pdt(pdpt_t pdpt, unsigned long pdpt_index, int is_kernal) {
    pdt_t pdt = (pdt_t) kmalloc(KERNAL_TB,1);
    unsigned long pdt_entry = (unsigned long) pdt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pdt_entry |= PT_U;
    }
    //set writtable and present bits
    pdt_entry |= (PT_P | PT_W);
    //pdpt_entry is physical address
    pdpt->PDPTE[pdpt_index] = pdt_entry;
    return (void *) pdt;
}


void* set_pt(pdt_t pdt, unsigned long pdt_index, int is_kernal) {
    pt_t pt = (pt_t) kmalloc(KERNAL_TB,1);
    unsigned long pt_entry = (unsigned long) pt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pt_entry |= PT_U;
    }
    //set writtable and present bits
    pt_entry |= (PT_P | PT_W);
    /*pdpt_entry is physical address*/
    pdt->PDTE[pdt_index] = pt_entry;
    return (void *) pt;
}


void *memset(void *s, int ch , unsigned long n) {
    char* tmp = s;
    while(n > 0) {
        *tmp = ch;
        tmp++;
        n--;
    }
    return s;
}




