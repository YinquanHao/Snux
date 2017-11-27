#include <sys/pagefault.h>

void page_fault_handler(struct regs *r)
{
    uint64_t error=r->err_code&0xF;
    uint64_t f_address;
    __asm volatile("mov %%cr2, %0" : "=r" (f_address));
    uint64_t phy_address=get_physical_addr(f_address);

    if(error&0x1){//the page is presented 
        if(error&0x2){// protection fault during write
            allocate_vir_page(f_address,0);
        }

    }
    else{//demand page
        demand_paging(f_address);
    }
}

void demand_paging(uint64_t faddr){
    uint64_t phy_page;
    task_struct *task = get_current_task();
    vma_struct *vma=task->mm->mmap;
    uint64_t phy_addr=get_physical_addr(faddr);
    phy_addr=phy_addr&PERM_MASK;
    pml4_t cur_pml4=(pml4_t)get_CR3();
    cur_pml4=(pml4_t)((VIRT_ST)|((uint64_t) cur_pml4)); 
    
    while(vma!=NULL){
        if(vma->type == 2&&faddr>=vma->start&&faddr+0x1000<=heap_end){//heap
            phy_page=allocate_page();
            user_process_mapping(faddr,phy_page,cur_pml4,0);
            memset(faddr,0,4096);
            vma->end=vma->end+0x1000;
            return;
        }
        if(vma->type==3&&faddr<=vma->end&&vma->start-vma->end-0x1000<=stack_end){
            //allocate_vir_page();
            //vma->end=new 
            //return;
            phy_page=allocate_page();
            user_process_mapping(vma->end-0x1000,phy_page,cur_pml4,0);
            memset(vma->end-0x1000,0,4096);
            vma->end=vma->end-0x1000;
            return;

        }
        vma=vma->next;
    }
}