#include <sys/pagefault.h>
#include <sys/process.h>
#include <sys/virtualmem.h>
#define vir_copy 0xFFFFFFFF80040000UL



void page_fault_handler(struct regs *r)
{
    __asm__ __volatile__("xchg %bx, %bx");
    uint64_t error=r->err_code & 0xF;
    uint64_t vaddr;
    __asm volatile("mov %%cr2, %0" : "=r" (vaddr));

    if(error&0x1){//the page is presented 
        if(error&0x2){// protection fault during write

            /*

            uint64_t phy_address=get_physical_addr_user(vaddr);
            allocate_vir_page(vaddr,0);
            uint64_t phy_page=allocate_page();
            


            uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,f_address);
            user_process_mapping(vir_copy,phy_page,pml4_addr,0);
            memset(vir_copy,0,4096);
            uint64_t phy_addr=get_physical_addr(vaddr);
            phy_addr=phy_addr&PERM_MASK;
            memcpy((void*)vir_copy,(void*)f_address,4096);
            user_process_mapping(vaddr,phy_addr,cur_pml4,0);
            vir_copy=vir_copy+0x1000;
            //user_process_mapping_v2()
            user_space_allocate(vir_copy);
            memset(vir_copy,0,4096);
            uint64_t phy_addr=get_physical_addr(f_address);
            phy_addr=phy_addr&PERM_MASK;
            memcpy((void*)vir_copy,(void*)f_address,4096);
            uint64_t pml4_addr=get_tb_virt_addr(PML4_LEVEL,f_address);
            user_process_mapping(f_address,phy_addr,pml4_addr,0);
            */
        }
    }else{//demand page
        demand_paging(vaddr);
        kprintf("finished demand");
    }
    kprintf("page fault handler error code %x",error);
    //while(1);

}

void demand_paging(uint64_t vaddr){
    uint64_t phy_page;
    task_struct *task = get_current_task();
    vma_struct *vma=task->mm->mmap;
    uint64_t pml4_addr;
    //pml4_t cur_pml4=(pml4_t)get_CR3();
    //cur_pml4=(pml4_t)((VIRT_ST)|((uint64_t) cur_pml4)); 
    while(vma!=NULL){

        if(vma->type == HEAP_VMA && vaddr >= vma->start && vaddr + 0x1000<= HEAP_LIMIT){//heap
            phy_page=allocate_page();
            pml4_addr=get_tb_virt_addr(PML4_LEVEL,vaddr);
            user_process_mapping(vaddr,phy_page,pml4_addr,0);
            memset(vaddr,0,4096);
            vma->end=vma->end+0x1000;
            return;
        }

        if(vma->type==STACK_VMA && vaddr<=vma->end && vma->end-0x1000>=USER_STACK_LIMIT){
            phy_page=allocate_page();
            pml4_addr=get_tb_virt_addr(PML4_LEVEL,vaddr);
            user_process_mapping(vaddr-0x1000,phy_page,pml4_addr,0);
            memset(vaddr-0x1000,0,4096);
            vma->end=vma->end-0x1000;
            return;
        }

        vma=vma->next;
    }
}