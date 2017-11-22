#include <sys/virtualmem.h>
#include <sys/physmem.h>
#include <sys/kprintf.h>
#include <sys/defs.h>

pml4_t PML4;
pml4_t PML41;

//physfree =  0x20c000
void init_kernalmem(unsigned long physfree){
    //kmalloc a Page for PML4
	PML4 = (pml4_t)kmalloc(KERNAL_TB,PAGE_SIZE);
}


void init_virt_phy_mapping() {
    map_kernel(0x20C000); 
    vir_phy_mapping(VIRT_ST+0xB8000,0xB8000);
    mapping_test();
    set_CR3((unsigned long) PML4);
}

void mapping_test(){
    pdpt_t pdpt_tab;
    pdt_t pdt_tab;
    pt_t pt_tab;
    pdpt_tab = (pdpt_t)((PML4->PML4E[511]&PERM_MASK));
    //kprintf("testing pdpt_tab %x \n",pdpt_tab->PDPTE[510]&PERM_MASK);
    pdt_tab = (pdt_t)(pdpt_tab->PDPTE[510]&PERM_MASK);
    //kprintf("testing pdt_tab %x \n",pdt_tab->PDTE[1]&PERM_MASK);
    pt_tab = (pt_t)(pdt_tab->PDTE[1]&PERM_MASK);
    //kprintf("testing pt_tab %x \n",pt_tab->PTE[0]&PERM_MASK);
}


void map_kernel(unsigned long map_size){
    unsigned long vir_st  = VIRT_ST+0x200000;
    unsigned long phy_st = 0x200000;
   // unsigned long phy_st = 0x20C000;60DB000
    //int k=0;
    while(phy_st<0x60DB000){
        vir_phy_mapping(vir_st,phy_st);
        vir_st += PAGE_SIZE; //*20000;
        phy_st += PAGE_SIZE; //*20000;
        //k+=PAGE_SIZE;
    }
}

/*returned a physical address for allocated one page size*/
void* kmalloc(int flag, unsigned int size){
    unsigned long res_addr = 0x0;
    switch(flag){
        case KERNAL_TB:
            res_addr = allocate_page();
            memset(res_addr,0,4096);
            break;
        case KERNAL_MEM:
            res_addr = allocate_page();
            memset(get_vir_from_phy(res_addr),0,4096);
            break;
        case USER_MEM:
            res_addr = allocated_page();
            memset(get_vir_from_phy(res_addr),0,4096);
            break;
        default:
            res_addr = 0x0;
    }
    return (void*)res_addr;
}


void user_process_vir_phy_mapping(uint64_t vir_addr, uint64_t phy_addr, pml4_t user_PML4){
    pdpt_t pdpt_tab;
    pdt_t pdt_tab;
    pt_t pt_tab;
    unsigned long entry_address;
    //(((vir_addr) >> 39) & 0x1FF);
    unsigned long pml4_index = (((vir_addr) >> 39) & 0x1FF);
    //printf("pml4_index %lu\n", pml4_index);
    unsigned long page_dir_pt_index  = (((vir_addr) >> 30) & 0x1FF);
    //printf("page_dir_pt_index %lu\n", page_dir_pt_index);
    unsigned long page_dir_index  = (((vir_addr) >> 21) & 0x1FF);
    //printf("page_dir_index %lu\n", page_dir_index);
    unsigned long page_tb_index =  (((vir_addr) >> 12) & 0x1FF);
    //printf("page_tb_index %lu\n", page_tb_index);
    unsigned long pml4_entry=user_PML4->PML4E[pml4_index];

     if(pml4_entry & PT_P ){                        //if the given entry presented in the table, use it
        entry_address=pml4_entry&PERM_MASK;    //mask the permission bits of the address  
        pdpt_tab=(pdpt_t)(entry_address);
    }else{
        pdpt_tab=(pdpt_t)set_pdpt(PML4,pml4_index,USER_MALLOC);
    }
    unsigned long pdpt_entry=pdpt_tab->PDPTE[page_dir_pt_index];
    if(pdpt_entry & PT_P ){ 
        entry_address=(pdpt_entry)&PERM_MASK;      
        pdt_tab=(pdt_t)(entry_address);
    }else{
        pdt_tab=(pdt_t)set_pdt(pdpt_tab,page_dir_pt_index,USER_MALLOC);
    }
    unsigned long pdt_entry=pdt_tab->PDTE[page_dir_index];
    if(pdt_entry & PT_P ){ 
        entry_address=(pdt_entry)&PERM_MASK;      
        pt_tab=(pt_t)(entry_address);
    }else{
        pt_tab=(pt_t)set_pt(pdt_tab,page_dir_index,USER_MALLOC);
    }
    unsigned long page_entry=phy_addr;
    page_entry=page_entry|PT_P|PT_U|PT_W; //set the page table flags
    pt_tab->PTE[page_tb_index]=page_entry;
   /// kprintf("pml4_index: %d page_tb_index: %d ",pml4_index, page_tb_index);
}



void vir_phy_mapping(unsigned long vir_addr, unsigned long phy_addr){
    pdpt_t pdpt_tab;
    pdt_t pdt_tab;
    pt_t pt_tab;
    unsigned long entry_address;
    //(((vir_addr) >> 39) & 0x1FF);
    unsigned long pml4_index = (((vir_addr) >> 39) & 0x1FF);
    //printf("pml4_index %lu\n", pml4_index);
    unsigned long page_dir_pt_index  = (((vir_addr) >> 30) & 0x1FF);
    //printf("page_dir_pt_index %lu\n", page_dir_pt_index);
    unsigned long page_dir_index  = (((vir_addr) >> 21) & 0x1FF);
    //printf("page_dir_index %lu\n", page_dir_index);
    unsigned long page_tb_index =  (((vir_addr) >> 12) & 0x1FF);
    //printf("page_tb_index %lu\n", page_tb_index);
    unsigned long pml4_entry=PML4->PML4E[pml4_index];
    
    if(pml4_entry & PT_P ){                        //if the given entry presented in the table, use it
        entry_address=pml4_entry&PERM_MASK;    //mask the permission bits of the address  
        pdpt_tab=(pdpt_t)(entry_address);
    }else{
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
    }
    unsigned long page_entry=phy_addr;
    page_entry=page_entry|PT_P|PT_U|PT_W; //set the page table flags
    pt_tab->PTE[page_tb_index]=page_entry;
   /// kprintf("pml4_index: %d page_tb_index: %d ",pml4_index, page_tb_index);
}


/*get virtual address from given physical address*/
unsigned long get_vir_from_phy(unsigned long phys_addr){
    return (phys_addr|VIRT_ST);
}


void* set_pdpt(pml4_t pml4, unsigned long pml4_index, int is_kernal) {
    pdpt_t pdpt = (pdpt_t) kmalloc(KERNAL_TB,1);
    //kprintf("pdpt table address %x \n", pdpt);
    unsigned long pdpt_entry = (unsigned long) pdpt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pdpt_entry |= PT_U;
    }

    //for test ring3
    pdpt_entry |= PT_U;

    //set writtable and present bits
    pdpt_entry |= (PT_P | PT_W);
    /*pdpt_entry is physical address*/
    pml4->PML4E[pml4_index] = pdpt_entry;
    return (void *) pdpt;
}


void* set_pdt(pdpt_t pdpt, unsigned long pdpt_index, int is_kernal) {
    //kprintf("set pdpt!!!!!!!!!!!! %x ,%d",pdpt,pdpt_index);

    pdt_t pdt = (pdt_t) kmalloc(KERNAL_TB,1);

    unsigned long pdt_entry = (unsigned long) pdt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pdt_entry |= PT_U;
    }

    //for test ring3
    pdt_entry |= PT_U;

    //set writtable and present bits
    pdt_entry |= (PT_P | PT_W);
    //pdpt_entry is physical address
    //kprintf("pdt table address %x \n", pdt_entry);
    pdpt->PDPTE[pdpt_index] = pdt_entry;
    //kprintf("pdt_entry %x \n",pdt_entry);
    return (void *) pdt;
}


void* set_pt(pdt_t pdt, unsigned long pdt_index, int is_kernal) {
    pt_t pt = (pt_t) kmalloc(KERNAL_TB,1);
    //kprintf("pt table address %x \n", pt);
    unsigned long pt_entry = (unsigned long) pt;
    //pdpt_entry -= VIR_START; // convert to physical address
    if (is_kernal == USER_MALLOC) {
        pt_entry |= PT_U;
    }

    //for test ring3
    pt_entry |= PT_U;


    //set writtable and present bits
    pt_entry |= (PT_P | PT_W);
    /*pdpt_entry is physical address*/
    pdt->PDTE[pdt_index] = pt_entry;
    return (void *) pt;
}


void set_CR3(pml4_t new_cr3){
    unsigned long base_pgdir_addr = (unsigned long)new_cr3;
    __asm volatile("mov %0, %%cr3":: "b"(base_pgdir_addr));

}

uint64_t get_CR3(){
    uint64_t res;
    __asm volatile("mov %%cr3, %0" : "=r"(res));
    return res;
}

void *set_user_addr_space(){
    //create a new space for new PML4 table in user_process(virtual addr)
    pml4_t new_PML4 = (pml4_t)get_vir_from_phy(allocate_page());
    //get the current PML4 table (virtual addr)
    pml4_t cur_PML4 = (pml4_t)get_vir_from_phy(get_CR3());
    //TODO YinquanHao  map the kernel page table for the process???
    int i=0;
   // for(i=0;i<512;i++){
        new_PML4->PML4E[511] = cur_PML4->PML4E[511];
    //}
    //new_PML4->PML4E = cur_PML4->PML4E;
    return (void*)new_PML4;
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




