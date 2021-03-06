#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
//#include <sys/idt.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/physmem.h>
// end store the last struct
task_struct* end;
task_struct* first;
task_struct* current;
extern page_t* physical_page_start;
extern page_t* free_pg_head;

uint64_t last_thread;
//tell which rsp to go when comes back from ring3
uint64_t kernel_rsp;
//list of pid
int pid[PROCESS_NUM];
int bgflag=0;
/*init the pid array*/
void init_pid(){
	int i;
	for(i =0;i<PROCESS_NUM;i++){
		pid[i]=0;
	}
}

/*scan the pid array get the first free index*/
int get_pid(){
	int i;
	for(i =0;i<PROCESS_NUM;i++){
		if(pid[i]==0){
			pid[i]=1;
			return i;
		}
	}
	return -1;
}

/*create the init_kthread, the init_kthread would take over the functionality of main thread*/
task_struct* create_init_kthread(){
	//kmalloc a task_struct for new kthread
	task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//create a stack for the newly created thread
	void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//make task->rsp points to the highest address of the stack
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);
	//assign an pid for the thread
	task->pid = get_pid();
	//make rip points to the initial function
	task->rip=(uint64_t)&init_thread_fn;
	//TODO @yinquanhao state and exit_status for the task (Need to modify later not correct)
	task->state = 3;
	task->exit_status = 0;
	//add task (initialize and keep track of the  first, end and current task structure)
	add_task(task);
	//make the rsp points to the stack pointer in the PCB
	__asm__ __volatile__ (	
		"movq %0, %%rsp"
		::"r"(task->rsp)
	);
	//save the task->rip to register rax and push it to the system's call_stack
	__asm__ __volatile__(
		"movq %0, %%rax;"\
		"pushq %%rax;"\
		::"r"(task->rip)
	);
	//asm ret instruction start execuating the thread
	__asm__ __volatile__ (
		"ret"
	);
	return task;
}


/*create new kthread*/
task_struct* create_new_kthread(void* thread){
	//kmalloc a task_struct for new kthread
	task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//create a stack for the newly created thread
	void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//make task->rsp points to the highest address of the stack
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);
	//assign an pid for the thread
	task->pid = get_pid();
	//make rip points to the initial function
	task->rip=(uint64_t) thread;
	//TODO @yinquanhao state and exit_status for the task (Need to modify later not correct)
	task->state = 3;
	task->exit_status = 0;
	//add task (initialize and keep track of the  first, end and current task structure)
	add_task(task);
	//push the rip to the kstack
	*task->kstack = thread;
	//fake we pushed some thing to the kstack, kstack grow downwords
	task->rsp -=15*8;
	//return new task
	return task;
}


/*add a task to run queue*/
void add_task(task_struct * task) {
	/*the case of the task is initial task*/
	if (first == NULL){
		first=task;
		current=task;
		end=task;
		task->next = task;
	}
	/*normal case for adding task*/
	else{
		end->next=task;
		end=end->next;
		end->next=first;
	}
}

void context_switch(task_struct *me,task_struct *next){

	//push all registers on current thread's stack
	__asm__ __volatile__ (
		"pushq %rax;"
		"pushq %rbx;"
		"pushq %rcx;"
		"pushq %rdx;"
		"pushq %rdi;"
		"pushq %rsi;"
		"pushq %rbp;"
		"pushq %r8;"
		"pushq %r9;"
		"pushq %r10;"
		"pushq %r11;"
		"pushq %r12;"
		"pushq %r13;"
		"pushq %r14;"
		"pushq %r15;"
	);

	//save rsp register in current thread's context
	__asm__ __volatile__ (	
		"movq %%rsp, %0"
		:"=r"(me->rsp)
	);
	//set cr3
	__asm volatile("mov %0, %%cr3":: "r"(next->cr3));

	//change the current process
	current = next;

	//also update the tss
	kernel_rsp = current->rsp;

	//change the rsp register with next thread's stack
	__asm__ __volatile__ (	
		"movq %0, %%rsp"::"r"(next->rsp)
	);

	//pop all the register values in kernel stack to registers
	__asm__ __volatile__ (
		"popq %r15;"
		"popq %r14;"
		"popq %r13;"
		"popq %r12;"
		"popq %r11;"
		"popq %r10;"
		"popq %r9;"
		"popq %r8;"
		"popq %rbp;"
		"popq %rsi;"
		"popq %rdi;"
		"popq %rdx;"
		"popq %rcx;"
		"popq %rbx;"
		"popq %rax;"
	);

	//return
	__asm__ __volatile__ (
		"ret"
	);

}

/*use this function as our main thread*/
void init_thread_fn(){
	//kprintf("enter init_thread_fn");
	//create a new thread which execute a binary of our bash
	task_struct* thread1 = create_user_process("bin/init");

	//switch to ring3 and start execute the binary
	switch_to_ring3(thread1);
	//TODO @YinquanHao Need some keep scheduling mechnism here
	while(1);
}

/*the dummy function for testing*/
void print_thread(){
	kprintf("Thread 1 \n");
	schedule();
	kprintf("Thread 2 \n");
	schedule();
	kprintf("Thread 3 \n");
	while(1);
}

//schedule next process
void schedule(){
	//get current process as temp
	task_struct* temp=current->next;
	while(temp){
		//get the process which is not current process and the state is not zombie and the pid!=0 (it's init thread cannot comes back!)
		if(temp!=current&&temp->state!=ZOMBIE&&(temp->pid)!=0){
			//switch to next function
			context_switch(current,temp);
			return;
		}else{
			//get next process
			temp=temp->next;
		}
	}
}


//create a user process
task_struct* create_user_process(char* filename){
	//find the binary file by giving name
	posix_header_t *hd = get_binary(filename);
	//create a new task_struct
    task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //create a new stack for the new process
    void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//make task->rsp points to the highest address of the stack
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);
	//set the task state to ready
	task->state = READY;
	//set the current working dir to rootfs/bin
    task->cur_dir = sys_opendir("rootfs/bin");
    //save the path to task_struct
    strcpy(task->cwd,"rootfs/bin");
    //kprintf("cwd: task->cwd %s \n",task->cwd);
    //int fd  = sys_open("rootfs/test/hello");
    //kprintf("the fd %d \n",fd);
    //int clo = sys_close(fd);
    //kprintf("the clo %d \n",clo);
    //char buf[100];
    //int read = sys_read(3,buf,100);
    task->sleep_time=0;
   	//get the pid for the process
    int pid=get_pid();
    //set the pid
    task->pid=pid;
    //get the prev_cr3 and store it in prev_cr3
    void* prev_cr3 = get_CR3();
    //create new pml4 and this pml4 is in kernel space
    void* new_PML4 = set_user_addr_space();
    //mask the new_PML4's address back to physical
    new_PML4 = ((uint64_t)new_PML4)^VIRT_ST;
    //set the task->cr3 to new created PML4 (physical)
    task->cr3 = new_PML4;
    //set the cr3 to new PML4
    set_CR3((uint64_t)new_PML4);

    task->vir_top = VMA_VA_ST;
    //allocate the user_space and map it into vir_top
    user_space_allocate(task->vir_top);
    //make the mm struct points to the  mm
    mm_struct *mm = (mm_struct *)(task->vir_top);
    //increment the vir_top as one pg size
    task->vir_top+=PAGE_SIZE;
    //set task
	task->mm = mm;
    //load the elf
    load_elf(task,(void*)(hd+1));
    //set the cr3 back to previous
    set_CR3((pml4_t)prev_cr3);
    //add task
    add_task(task);

    return task;
}


task_struct* get_current_task(){
	return current;
}


void switch_to_ring3(task_struct* task){
	//kprintf("ring3");	
	kernel_rsp = task->kstack;
//uint64_t uf=(uint64_t)&user_func;
//uint64_t rsp_cp;
	current = task;
//kprintf("  %x,%x,%x  ",task->rsp,task->rip,task->cr3);
//while(1);
	set_tss_rsp(task->kstack);
	set_CR3(task->cr3);
	__asm__ __volatile__(
		"cli;"
    	"movq %0,%%rax;"
    	"pushq $0x23;"
   		"pushq %%rax;"
   		"pushfq;"
   		"popq %%rax;"
       	"orq $0x200, %%rax;"
       	"pushq %%rax;"
    	"pushq $0x2B;"
    	"pushq %1;"
    	::"r"(task->rsp),"r"(task->rip)
    	);
	__asm__ __volatile__(
		"iretq;"
		);
}

//wrapper function to allocate a user page in userspace
uint64_t user_space_allocate(uint64_t viraddr){
	//return the physical addr of the allocated page
	uint64_t phyaddr = allocate_page();
	//get the physical addr of current cr3
    uint64_t cur_pml4 = get_CR3();
    //get pml4's virtual address
    uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,viraddr);
    //do the mapping
    user_process_mapping(viraddr,phyaddr,pml4_addr,0);
    //mem set the space we just newly created
    memset(viraddr,0,PAGE_SIZE);
    //return the physical address
    return phyaddr;
}


void sys_exit(int status) {
	// current task's parent
    task_struct *parent;
    //set current task as zombie
    current->state = ZOMBIE;
    //current->ret_val = status;
    // get parenet's task
    if ((current->ppid!=0)) {
    	parent  = get_task_by_pid(current->ppid);
        // if found task_struct of parent process
        parent->waitpid = 0;
        //check if parent's task is blocked
        if(bgflag==1){
        	bgflag=0;
        	context_switch(current,parent);
        	return;
        }
        if (parent->state == SLEEPING || parent->state == WAIT) {
        	//set parent's task as ready
            parent->state = READY;
             //do the context switch
            context_switch(current,parent);
            return;
        }
        //remove current task from the task linked list
    } else {
        kprintf("cannot find parent\n");
    }
    //schedule next process
    schedule();
}


//fork a child process
uint64_t fork(){
	//save the current rsp to current->rsp
	__asm__ __volatile__ (	
		"movq %%rsp, %0"
		:"=r"(current->rsp)
		);
	//set the parent points to current process
	task_struct* parent=current;
	//allocate a new space for the child process	
	task_struct* child=(task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//assign a new pid to child process
	child->pid=get_pid();
	//assign the parent pid of child process as parent's pid
	child->ppid=parent->pid;
	//TODO yinquanhao need to fix copy child table
	child->vir_top = parent->vir_top;

	memcpy(&(child->fd[0]),&(current->fd[0]),(sizeof(current->fd[0])* 100));
	//copy the parent's page table to child's page table
	copy_child_table(child);
	//copy the vma
	copy_child_mm(child);
	// make temp process structure pointer
	task_struct *temp;
	//add child task to the task linked loop
	add_task(child);
	//set child process's waitpid as 0
	child->waitpid=0;
	//set child process''s state to READY
	child->state=READY;
	//kprintf("%x,%x",syscall_rsp,current->rsp);
	//allocate a new one page for child process's stack
    void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));	
	//memset the stack to provent garbage
	memset(stack,0,4096);
	//let child process's rsp points to the top of the new allocated stack
	child->rsp =child->kstack = stack +0x1000 -16;	
	//child->init_k=child->kstack;
	//child->rip=syscall_rip;
	//set pkstack as the lowest address of parent process's kstack
	uint64_t pkstack=(uint64_t)(parent->kstack)-0x1000+16;  //0xffffffff8035f070
	
	//kprintf("%x,%x",pkstack,child->kstack);
	//memcpy the child process same as parent's
	memcpy((void*)stack,(void*)(pkstack),0x1000-16);

	if(current == parent) {
		//check the offset(usage) of parent process's kernel stack
		uint64_t offset=(uint64_t)current->kstack-(uint64_t)current->rsp;
		//adjust child process's rsp
		child->rsp-=offset;
		//set the parent pid
		child->ppid = current->pid;
		//set up child stack push current register to the child stack
		set_up_child_stack(child);

		//kprintf("rsp:%x",child->rsp);

		//set the current cr3 to parent's cr3
		set_CR3((struct PML4 *)parent->cr3);
		//the return value for child process
		uint64_t chld_ret=child->pid;
		//save current rsp to parent's rsp
		__asm__ __volatile__ (	
			"movq %%rsp, %0"
			:"=r"(parent->rsp)
			);
		//set tss
		set_tss_rsp(parent->rsp);
		//TODO @yinquanhao fix the tss not working issue and get rid of kernel_rsp
		kernel_rsp = parent->rsp;
		//return the process id
		return chld_ret;

	}else{
		//set cr3 to child process's page table
		set_CR3(child->cr3);
		//save the rsp to child_>rsp
		__asm__ __volatile__ (	
			"movq %%rsp, %0"
			:"=r"(child->rsp)
		);
		//set up the tss
		set_tss_rsp(child->rsp);
		//set up the kernel stack
		kernel_rsp = child->rsp;
		//return 0
		return 0;
	}
}


void __attribute__((optimize("O0"))) copy_child_table(task_struct *child){
    //allocate the user_space and map it into vir_top
    uint64_t phy_addr = user_space_allocate(child->vir_top);
    //keep an copy of pml4_c physical address
    uint64_t pml4_c_phy = phy_addr;
    //make pml4_c points to child->vir_top
    pml4_t pml4_c = (pml4_t)(child->vir_top);
    //increment the vir_top as one pg size
    child->vir_top+=PAGE_SIZE;
    //enable self ref
    pml4_c->PML4E[510] = phy_addr| PT_P;;
    //get virtual address of parent's pml4
    pml4_t pml4_p = get_tb_virt_addr(PML4_LEVEL,0);
    //copy the kernel mem
    pml4_c->PML4E[511] = pml4_p->PML4E[511];

    child->cr3 = pml4_c_phy;


    pdpt_t pdpt_c;
    pdpt_t pdpt_p;
    pdt_t pdt_p;
    pdt_t pdt_c;
    pt_t pt_p;
    pt_t pt_c;
    int i=0;
    for(;i<510;i++){
    	//if the entry of pml4_p presents
    	uint64_t pml4_entry =pml4_p->PML4E[i];
    	if(pml4_entry & PT_P){
    		//allocate a new pdpt_c
    		phy_addr = user_space_allocate(child->vir_top);
    		//set pdpt_c to the entry
    		pml4_c->PML4E[i] = phy_addr|PT_P|PT_U|PT_W;
    		//get the pdpt_c virtual addr
    		pdpt_c = (pdpt_t)(child->vir_top);
    		//get pdpt_p's virtual addr
    		pdpt_p = (pdpt_t)(PDPT_SELF|(uint64_t)i<<12);
    		//increment the vir_top
    		child->vir_top+=PAGE_SIZE;
    		int j=0;
    		for(;j<512;j++){
    			uint64_t pdpt_entry = pdpt_p->PDPTE[j];
    			//if the pdpt entry presents
    			if(pdpt_entry & PT_P){
    				//allocate a new space for pdt_c
    				phy_addr = user_space_allocate(child->vir_top);
    			
    				pdpt_c->PDPTE[j] = phy_addr|PT_P|PT_U|PT_W;
    				//get pdt_c's virtual address
    				pdt_c = (pdt_t)(child->vir_top);
    				//get virtual address of parent pdt
    				pdt_p = (pdt_t)(uint64_t)(((uint64_t)i<<21)|((uint64_t)j<<12)|PDT_SELF);
    				//increment the vir_top
    				child->vir_top+=PAGE_SIZE;
    				int k=0;
    				for(;k<512;k++){
    					//if the pdt entry presents
    					uint64_t pdt_entry = pdt_p->PDTE[k];
    					if(pdt_entry & PT_P){
    						phy_addr = user_space_allocate(child->vir_top);
    						//set pdt_c's entry as the physical address of pt_c
    						pdt_c->PDTE[k] = phy_addr|PT_P|PT_U|PT_W;
    						//get pt_c
    						pt_c = (pt_t)(child->vir_top);
    						//get parent's pt table
    						pt_p = (pt_t)(((uint64_t)i<<30)|((uint64_t)j<<21)|((uint64_t)k<<12)|PT_SELF);
    						//increment the vir_top
    						child->vir_top+=PAGE_SIZE;
    						int m=0;
    						for(;m<512;m++){
    							uint64_t pt_entry = pt_p->PTE[m];
    							if(pt_entry & PT_P){
    								//copy the value
    								pt_c->PTE[m] = pt_p->PTE[m];
    							}
    						}
    					}
    				}
    			}
    		}
    	}
    }
    set_CR3(pml4_c_phy);
    //while(1);
}



void copy_child_mm(task_struct *child){
	/*copy the parent vma*/

	//set child's vir_top back to it's parents
	//child->vir_top = current->vir_top;
	//allocate an new user space
	user_space_allocate(child->vir_top);
	//make the new created user space points to child_mm
	mm_struct *child_mm=(mm_struct*)(child->vir_top);
	//increment the child process's vir_top
	child->vir_top+=0x1000;
	//set child->mm
	child->mm=child_mm;
	//get head of parent's vma
	vma_struct *parent_vma=current->mm->mmap;
	//create a pointer points to child_vma
	vma_struct *child_vma=NULL;
	//create a vma pointer as prev
	vma_struct *prev;
	//set is first flog
	uint64_t is_first=1;

	uint64_t phyaddr;
	//set cr3 back to the cuurent->cr3 (Do we really need that?)
	//set_CR3(current->cr3);
	//memcpy the parent->mm
	memcpy((void*)child->mm,(void*)current->mm,0x1000);
	// if parent has vmas
	while(parent_vma!=NULL){
		//if this is the head of vma list
		if(is_first==1){
			//allocate a new user_space 
			phyaddr = user_space_allocate(child->vir_top);
			vma_struct *child_vma=(vma_struct*)(child->vir_top);
			child->vir_top+=0x1000;
			//copy the attributes lazy cpy here
			child_vma->start=parent_vma->start;
			child_vma->end=parent_vma->end;
			child_vma->flags=parent_vma->flags;
			child_vma->type=parent_vma->type;
			if(parent_vma->file!=NULL){
				user_space_allocate(child->vir_top);
				struct file* child_vma_file =(vma_struct*)(child->vir_top);
				child->vir_top+=0x1000;
				memcpy(child_vma_file,parent_vma->file,sizeof(struct file));
				child_vma->file = child_vma_file;
			}else if(parent_vma->type==STACK){

				uint64_t size = (uint64_t)parent_vma->start - (uint64_t)parent_vma->end;


				memcpy(child_vma,parent_vma,(uint64_t)((uint64_t)parent_vma->start-(uint64_t)parent_vma->end));

			}else if(parent_vma->type==HEAP){

				uint64_t size = parent_vma->end - parent_vma->start;
				size = size/PAGE_SIZE;
				while(size){
					user_space_allocate(child->vir_top);
					child->vir_top+=PAGE_SIZE;
					size--;
				}

				memcpy(child_vma,parent_vma,(uint64_t)((uint64_t)parent_vma->end-(uint64_t)parent_vma->start));
			}
			//set is_first flag back to 0
			is_first=0;
			//set mm_map

			child->mm->mmap=child_vma;

			child_vma->next = parent_vma->next;

			child_vma->mm = child->mm;


			uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,0);
    		//kprintf("parent process vma %x",(uint64_t)parent_vma);
    		user_process_mapping((uint64_t)parent_vma,phyaddr,pml4_addr,0);

    		set_CR3(child->cr3);
			//set prev
			prev=child_vma;

		}else{
			phyaddr = user_space_allocate(child->vir_top);
			vma_struct *child_vma=(vma_struct*)(child->vir_top);
			child->vir_top+=0x1000;
			child_vma->start=parent_vma->start;			
			child_vma->end=parent_vma->end;
			child_vma->flags=parent_vma->flags;
			child_vma->type=parent_vma->type;
			//link the vma list
			if(parent_vma->file!=NULL){
				user_space_allocate(child->vir_top);
				struct file* child_vma_file =(vma_struct*)(child->vir_top);
				child->vir_top+=0x1000;
				memcpy(child_vma_file,parent_vma->file,sizeof(struct file));
				child_vma->file = child_vma_file;
			}else if(parent_vma->type==STACK){
				uint64_t size = (uint64_t)parent_vma->start-(uint64_t)parent_vma->end;



				memcpy(child_vma,parent_vma,(uint64_t)((uint64_t)parent_vma->start-(uint64_t)parent_vma->end));
			}else if(parent_vma->type==HEAP){

				uint64_t size = parent_vma->end - parent_vma->start;
				size = size/PAGE_SIZE;
				while(size){
					user_space_allocate(child->vir_top);
					child->vir_top+=PAGE_SIZE;
					size--;
				}

				memcpy(child_vma,parent_vma,(uint64_t)((uint64_t)parent_vma->end-(uint64_t)parent_vma->start));
			}
			child_vma->mm = child->mm;

			child_vma->next = parent_vma->next;

			prev->next=child_vma;


			uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,0);
    		//kprintf("parent process vma %x",(uint64_t)parent_vma);
    		user_process_mapping((uint64_t)parent_vma,phyaddr,pml4_addr,0);
    		set_CR3(child->cr3);

			prev=child_vma;
		}
		//iterator to parent's next_vma
		parent_vma=parent_vma->next;
	}
	//while(1);
}


int sys_dowait4(uint64_t c_pid,uint64_t* status, uint64_t options){
	if(c_pid>0){// wait for specific process
		current->state=SLEEPING;
//kprintf("current:%x",current->pid);
		current->waitpid=c_pid; //wait for this pid
//kprintf("next:%x",current->next->pid);
//while(1);	
		schedule();
	}
	return c_pid;
}

task_struct* get_task_by_pid(uint64_t pid){
	task_struct* temp=current;
	while(temp){
		if(temp->pid==pid){
			return temp;
		}
		temp=temp->next;
	}
	return NULL;
}

void wake_up(task_struct* task){
	task->waitpid=0;
	task->state=READY;
}



// setup child stack
void set_up_child_stack(task_struct *child){
	uint64_t parent_rsp;
	child->rsp-=8;

	__asm__ __volatile__ (	
		"movq %%rsp, %0"
		:"=r"(parent_rsp)
		);

	//change the rsp register with next thread's stack
	__asm__ __volatile__ (	
		"movq %0, %%rsp"::"r"(child->rsp)
	);

	//push all registers on current thread's stack
	__asm__ __volatile__ (
		"pushq %rax;"
		"pushq %rbx;"
		"pushq %rcx;"
		"pushq %rdx;"
		"pushq %rdi;"
		"pushq %rsi;"
		"pushq %rbp;"
		"pushq %r8;"
		"pushq %r9;"
		"pushq %r10;"
		"pushq %r11;"
		"pushq %r12;"
		"pushq %r13;"
		"pushq %r14;"
		"pushq %r15;"
	);

	__asm__ __volatile__ (	
		"movq %%rsp, %0"
		:"=r"(child->rsp)
		);

	__asm__ __volatile__ (	
		"movq %0, %%rsp"::"r"(parent_rsp)
	);

}



void __attribute__((optimize("O0"))) sys_sleep(uint64_t stime){
	current->state=SLEEPING;
	//kprintf("%d\n",stime);
	current->sleep_time=stime*1000;
	//schedule();	
	if(current->next->pid==0||current->next==NULL){
		while(current->sleep_time>0){
		}
		kprintf("return");
		return;
	}else{
		schedule();
	}
	return;
}

void __attribute__((optimize("O0"))) background(int pid){
	//kprintf("   %d\n",current->pid );
	bgflag=1;
	current->state=SLEEPING;
	current->sleep_time=3000;
	while(current->sleep_time>0){
		//kprintf("%d",current->sleep_time);
	}
	//kprintf("out\n");
	current->state=READY;
	task_struct* t1=get_task_by_pid(pid);
	task_struct* t2=get_task_by_pid(current->pid);
	context_switch(t2,t1);	
	return;
}


void clear_sleep(){
	task_struct* temp=current;
	while(temp){
		if(temp->pid!=1&&temp->state==SLEEPING&&temp->sleep_time>=0){
			//kprintf("%d",temp->sleep_time);
			temp->sleep_time-=1000;
			if(temp->sleep_time<=0){
				temp->sleep_time=0;
				temp->state=READY;
				//kprintf("back");
			}						
			//break;
		}
		temp=temp->next;
		//kprintf("%d",temp->pid);	
		if(temp==current){return;}
	}
	return;
}


void task_list(){
	task_struct*temp=current;
	kprintf("%x",temp->next->pid);
	char* res[10]={0};
	kprintf("\npid   ppid   state  sleep_time");
	while(temp){
		if(temp->state==RUNNING){
			strcpy(res,"RUNNING");
		}
		else if(temp->state==SLEEPING){
			strcpy(res,"SLEEPING");
		}
		else if(temp->state==READY){
			strcpy(res,"READY");
		}		

		else if(temp->state==WAIT){
			strcpy(res,"WAIT");
		}		
		else if(temp->state==ZOMBIE){
			strcpy(res,"ZOMBIE");
		}
		kprintf("\n%d|    %d|      %s|      %d",temp->pid,temp->ppid,res,temp->sleep_time);
		if(temp->next==current){
			return ;
		}
		temp=temp->next;
	}	
}

uint64_t sys_getppid(){	
	return current->ppid;
}

void kill_task(uint64_t pid){
	task_struct *temp=current;
	task_struct *prev=current;
	task_struct *temp_parent;
	kprintf("%x",pid);
	//the task to kill
	while(temp){
		if(temp->pid==pid){
			break;
		}
		temp=temp->next;
		if(temp==current){//fails to find threa-1task
			kprintf("not found");
			return -1;
		}
	}
	//if has parent
	if(temp->ppid){
		temp_parent=get_task_by_pid(temp->ppid);
		if(temp_parent->waitpid==temp->pid&&temp_parent->waitpid==-1){
			temp_parent->waitpid=0;
			temp_parent->state=READY;
		}
	}

	while(prev){// find child
		if(prev->ppid==temp->pid){
			prev->ppid=0;
		}
		prev=prev->next;		
		if(prev==current){//end of search 
			break;
		}
	}	

	// remove from the list
	while(prev){// find the task prev to the killed task
		if(prev->next==temp){
			break;
		}
		prev=prev->next;		
		if(prev==current){//fails to find the task
			break;
		}
	}
	if(temp->next==NULL){
		//shutdown the os?
		//schedule();
	}
	else if(temp->next==prev){
		//two tasks
		prev->next=NULL; 
	}
	else{
		prev->next=temp->next;
	}

	// free vma, unmap and free the memory
	//int tt=allocate_page();
	//set_CR3(temp->cr3);
	mm_struct *temp_mm=temp->mm;
	vma_struct *vma_temp=temp_mm->mmap;
	uint64_t vaddr_start,vaddr_end;
	//uint64_t vaddr;
	while(vma_temp){
		vaddr_start=vma_temp->start;
		vaddr_end=vma_temp->end;
		while(vaddr_start<vaddr_end){
			freeVaddr(vaddr_start);
			vaddr_start+=0x1000;
		}
		vma_temp=vma_temp->next;
	}
	freeVaddr(temp->mm);
	freeVaddr(temp->kstack);
	freeVaddr(temp);
	set_CR3(current->cr3);
	//int tt2=allocate_page();
	//kprintf("   %x   %x    \n",tt,tt2);
	return ;
}

void freeVaddr(uint64_t vaddr){
	//set_CR3(temp->cr3);
	uint64_t phy=get_physical_addr_user(vaddr);
	phy=phy&PERM_MASK;
	//set_CR3(current->cr3);
	uint64_t index=phy/0x1000;
	//index=index-780;
	page_t *page_tmp;
	page_tmp=(page_t*)(physical_page_start)+index;
	page_tmp->ref_ct=0;
	page_tmp->occup=PG_FREE;
	//memset()
	page_tmp->next=free_pg_head->next;	
	free_pg_head->next=page_tmp;
	//kprintf(" free%x ",index);
}

int sys_wait(uint64_t* status){
	int c_pid;
	current->state=WAIT;
	current->waitpid=-1;//wait for all children
	schedule();
	task_struct* temp=current;
	while(temp){
		if(temp->state==ZOMBIE&&temp->ppid==current->pid){
			c_pid=temp->pid;			
			break;
		}
		temp=temp->next;
		if(temp==current){
			return -1;
		}
	}
	return c_pid;
}
