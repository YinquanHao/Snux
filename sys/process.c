#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
//#include <sys/idt.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
// end store the last struct
task_struct* end;
task_struct* first;
task_struct* current;
uint64_t last_thread;

uint64_t kernel_rsp;
//register int *foo asm ("rsp");



int pid[PROCESS_NUM];

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
	task->state = 1;
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
	task->state = 1;
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
		end=task;
		end->next=first;
	}
}

void context_switch(task_struct *me,task_struct *next){
	current = current->next;
	//__asm__ __volatile__("xchg %bx, %bx");

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
	
	//change the rsp register with next thread's stack
	__asm__ __volatile__ (	
		"movq %0, %%rsp"::"r"(next->rsp)
	);

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

	__asm__ __volatile__ (
		"ret"
	);

}

/*use this function as our main thread*/
void init_thread_fn(){
	kprintf("enter init_thread_fn");
	//while(1);
	task_struct* thread1 = create_user_process("bin/test");
	//while(1);
	//task_struct* thread1 = create_new_kthread(print_thread);
	//task_struct *u1=create_user_process();
	//task_struct *u2=create_user_process();
	//schedule();
	//while(1);
	switch_to_ring3(thread1);
	//switch_to_ring3(current);
	//kprintf("%d",u1->pid);
	//schedule();
	/*kprintf("xxxxxxxxxxxxxxxxxxxxxx");
	schedule();
	kprintf("bbbbbbbbbbbbbbbbbbbbbbb");
	schedule();*/
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

/*schedule function*/
void schedule(){
	context_switch(current,current->next);
}

task_struct* create_user_process(char* filename){

	posix_header_t *hd = get_binary(filename);

    task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));

    void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//make task->rsp points to the highest address of the stack
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);
	

/*
	char buf[50] ="rootfs/";
	char file_cp[50] = "\0";
	strcpy(file_cp,filename);
	kprintf("file_cp %s \n",file_cp);
	char* token;
	char* last;
	last = token = strtok(file_cp, "/");
	for (;(token = strtok(NULL, "/")) != NULL; last = token);
	kprintf("len   %s\n", last);
	kprintf("filename %s\n",filename);
	filename = strtok(filename,last);
	kprintf("filename %s\n",filename);
	strcat(buf,filename);
	kprintf("filename %s\n",buf);
	kprintf("length %d\n",strlen(buf));
	buf[strlen(buf)-1] = '\0';
	kprintf("filename %s\n",buf);
	task->cur_dir = sys_opendir("rootfs/bin");
	//kprintf("task->cur_dir %s \n",task->cur_dir);
*/
    task->cur_dir = sys_opendir("rootfs/bin");
    strcpy(task->cwd,"rootfs/bin");
    kprintf("cwd: task->cwd %s \n",task->cwd);
    //int fd  = sys_open("rootfs/test/hello");
    //kprintf("the fd %d \n",fd);
    //int clo = sys_close(fd);
    //kprintf("the clo %d \n",clo);
    //char buf[100];
    //int read = sys_read(3,buf,100);
   	//get the pid for the process
    int pid=get_pid();
    //set the pid
    task->pid=pid;
    //set the rsp
    //kprintf("the fd %d \n",fd);
    //kprintf("the fd %d \n",read);
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
    //set the virtual_addr in  task as the HEAP_LIMIT
    //PML4_SEL

    uint64_t a = get_tb_virt_addr(PML4_LEVEL,HEAP_LIMIT);

    //kprintf("%x",a);

    uint64_t b = get_physical_addr(a);

    //kprintf("%x",b);

    task->vir_top = VMA_VA_ST;
    //allocate the user_space and map it into vir_top
    user_space_allocate(task->vir_top);
    //make the mm struct points to the  mm
    mm_struct *mm = (mm_struct *)(task->vir_top);
    //increment the vir_top as one pg size
    task->vir_top+=PAGE_SIZE;
    //set task
	task->mm = mm;


/*

	//allocate a kernel stack in user_space and map the phys addr to task->virtop
	user_space_allocate(task->vir_top);
    //make the stack points to task->vir_top
    void* stack=(void *)(task->vir_top);
    //increment the virtop as one pg size
    task->vir_top+=PAGE_SIZE;
    //points to the kstack to the highest addr of kernel stack
    task->kstack=((uint64_t)stack +0x1000-16);

*/




    //load the elf
    //kprintf("size of %d",sizeof(posix_header_t));
    load_elf(task,(void*)(hd+1));
    //set the cr3 back to previous
    set_CR3((pml4_t)prev_cr3);
    //add task
    add_task(task);
    return task;
}
/*task_struct* create_test_process(){
	 task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	 void* stack=(void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	 int pid=get_pid();

}*/


task_struct* get_current_task(){
	return current;
}




void switch_to_ring3(task_struct* task){
	kprintf("ring3");	
	kernel_rsp = current->kstack;
	uint64_t uf=(uint64_t)&user_func;
	uint64_t rsp_cp;
	current = task;
	//kprintf("  %x,%x,%x  ",task->rsp,task->rip,task->cr3);
	//while(1);
	set_tss_rsp(task->kstack);
	set_CR3(task->cr3);
	__asm__ __volatile__("movq %%rsp,%0" : "=r"(rsp_cp));
	__asm__ __volatile__("xchg %bx, %bx");
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

/*void thread2_func(){
	kprintf(" a");
	switch_to_ring3(current);
	while(1);
}*/

void user_func(){
	//int i=1;
	kprintf("uuuu ");

	//switch_to_ring3(current);
	while(1);
}

void user_space_allocate(uint64_t viraddr){
	//return the physical addr of the allocated page
	uint64_t phyaddr = allocate_page();
	//get the physical addr of current cr3
    uint64_t cur_pml4 = get_CR3();
    
    uint64_t pml4_addr  = get_tb_virt_addr(PML4_LEVEL,viraddr);
    
    user_process_mapping(viraddr,phyaddr,pml4_addr,0);
    //mem set the space we just newly created
    memset(viraddr,0,PAGE_SIZE);
}


void do_exit(int status) {
    //task_struct *parent;
    //current->task_state = TASK_ZOMBIE;
    current->exit_status = status;

    /*
     * Check if parent process is suspended (by calling waitpid())
     * if yes, wake parent process
     */
    /*
    if ((parent = find_task_struct(current->ppid))) {
        // if found task_struct of parent process
        parent->wait_pid = current->pid;
        if (parent->task_state == TASK_BLOCKED) {
            parent->task_state = TASK_READY;
        }
    } else {
        // if parent process exits
        printf("cannot find parent process of pid %d (may terminated)\n",
                current->ppid);
    }
    */
    // current exited, so doesn't need push registers
    schedule();
}