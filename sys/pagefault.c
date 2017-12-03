#include <sys/pagefault.h>
#include <sys/process.h>
#include <sys/virtualmem.h>
#define vir_copy 0xFFFFFFFF80040000UL



void page_fault_handler(struct regs *r)
{
   /*
    __asm__ __volatile__("xchg %bx, %bx");
    uint64_t error=r->err_code&0xF;
    uint64_t f_address;
    __asm volatile("mov %%cr2, %0" : "=r" (f_address));
    uint64_t phy_address=get_physical_addr(f_address);

    if(error&0x1){//the page is presented 
        if(error&0x2){// protection fault during write
/*            allocate_vir_page(f_address,0);
            uint64_t phy_page=allocate_page();
            //pml4_t cur_pml4=(pml4_t)get_CR3();
            uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,f_address);
            user_process_mapping(vir_copy,phy_page,pml4_addr,0);
            memset(vir_copy,0,4096);
            uint64_t phy_addr=get_physical_addr(vaddr);
            phy_addr=phy_addr&PERM_MASK;
            memcpy((void*)vir_copy,(void*)f_address,4096);
            user_process_mapping(vaddr,phy_addr,cur_pml4,0);
            vir_copy=vir_copy+0x1000;*/
            //user_process_mapping_v2()
    /*
            user_space_allocate(vir_copy);
            memset(vir_copy,0,4096);
            uint64_t phy_addr=get_physical_addr(f_address);
            phy_addr=phy_addr&PERM_MASK;
            memcpy((void*)vir_copy,(void*)f_address,4096);
            uint64_t pml4_addr=get_tb_virt_addr(PML4_LEVEL,f_address);
            user_process_mapping(f_address,phy_addr,pml4_addr,0);
        }

    }
    else{//demand page
        demand_paging(f_address);
    }*/
    kprintf("enterbefeufiuewnfiwenifnweinfiwenfiwen");

}

void demand_paging(uint64_t faddr){
    uint64_t phy_page;
    task_struct *task = get_current_task();
    vma_struct *vma=task->mm->mmap;
    uint64_t phy_addr=get_physical_addr(faddr);
    phy_addr=phy_addr&PERM_MASK;
    int64_t pml4_addr;
    //pml4_t cur_pml4=(pml4_t)get_CR3();
    //cur_pml4=(pml4_t)((VIRT_ST)|((uint64_t) cur_pml4)); 
    
    while(vma!=NULL){
        if(vma->type == 2&&faddr>=vma->start&&faddr+0x1000<=heap_end){//heap
            phy_page=allocate_page();
            pml4_addr=get_tb_virt_addr(PML4_LEVEL,faddr);
            user_process_mapping(faddr,phy_page,pml4_addr,0);
            memset(faddr,0,4096);
            vma->end=vma->end+0x1000;
            return;
        }
        if(vma->type==3&&faddr<=vma->end&&vma->start-vma->end-0x1000<=stack_end){
            //allocate_vir_page();
            //vma->end=new 
            //return;
            phy_page=allocate_page();
            pml4_addr=get_tb_virt_addr(PML4_LEVEL,faddr);
            user_process_mapping(vma->end-0x1000,phy_page,pml4_addr,0);
            memset(vma->end-0x1000,0,4096);
            vma->end=vma->end-0x1000;
            return;

        }
        vma=vma->next;
    }
}