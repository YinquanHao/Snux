#include <sys/pagefault.h>
#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/physmem.h>


void page_fault_handler(struct regs *r)
{
    kprintf(" pf14");
    //__asm__ __volatile__("xchg %bx, %bx");
    uint64_t error=r->err_code & 0xF;
    uint64_t vaddr;
    __asm volatile("mov %%cr2, %0" : "=r" (vaddr));

    if(error&0x1){//the page is presented 
        if(error&0x2){// protection fault during write
            uint64_t ref_ct;
            uint64_t vaddr_masked=vaddr&PERM_MASK;
            uint64_t index=(uint64_t)(get_physical_addr_user(vaddr_masked)/0x1000);
            ref_ct=get_ref_ct(index);
            if(ref_ct==1){//set write bits for the entry
                set_pml4e_bits(vaddr,PT_W);
                set_pdpte_bits(vaddr,PT_W);
                set_pdte_bits(vaddr,PT_W);
                set_pte_bits(vaddr,PT_W);
            }
            else{//allocate copy
                uint64_t vir_copy;
                task_struct *task = get_current_task();
                vma_struct *vma=task->mm->mmap;
/*                while(vma->type!=HEAP_VMA&&vma!=NULL){
                    vma=vma->next;
                }
                if(vma->end+0x1000<HEAP_LIMIT){
                    vir_copy=vma->end;
                    vma->end+=0x01000;
                }*/
                vir_copy=task->vir_top;
                user_space_allocate(vir_copy);
                task->vir_top+=0x1000;
                memset(vir_copy,0,4096);
                uint64_t phy_addr=get_physical_addr_user(vir_copy);
                phy_addr=phy_addr&PERM_MASK;
                memcpy((void*)vir_copy,(void*)vaddr_masked,4096);
                uint64_t pml4_addr=get_tb_virt_addr(PML4_LEVEL,vaddr_masked);
                user_process_mapping(vaddr_masked,phy_addr,pml4_addr,0);
                set_pml4e_bits(vaddr,PT_W);
                set_pdpte_bits(vaddr,PT_W);
                set_pdte_bits(vaddr,PT_W);
                set_pte_bits(vaddr,PT_W);
            }
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