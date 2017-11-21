#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
// end store the last struct
task_struct* end;
task_struct* first;
task_struct* current;
uint64_t last_thread;
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

task_struct* create_user_process(){
    task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    void* stack=(void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //task->mm=(mm_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1)); 
    int pid=get_pid();
    task->pid=pid;
    task->rsp=task->kstack=((uint64_t)stack +0x1000-16);
    //task->rip=(uint64_t)&thread2_func;
    //task->cr3=get_CR3();
    *task->kstack=&user_func;
    task->rsp -=15*8;
    add_task(task);

    return task;
}


void switch_to_ring3(task_struct* task){
	uint64_t uf=(uint64_t)&user_func;
	uint64_t rsp_cp;
	__asm__ __volatile__("movq %%rsp,%0" : "=r"(rsp_cp));
	__asm__ __volatile__(
		"cli;"
     	"movq %0,%%rax;"
     	"pushq $0x23;"
    	"pushq %%rax;"
    	"pushfq;"
     	"pushq $0x2B;"
     	"pushq %1;"
     	::"r"(rsp_cp),"r"(uf)

	);
	__asm__ __volatile__(
		"iretq;"
	);

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



	
	/*uncomment for testing ring3*/
	task_struct* thread1 = create_user_process();
	schedule();




	/*uncomment for testing context switch*/
	/*
	task_struct* thread1 = create_new_kthread(print_thread);
	kprintf("xxxxxxxxxxxxxxxxxxxxxx");
	schedule();
	kprintf("bbbbbbbbbbbbbbbbbbbbbbb");
	schedule();
	set_user_addr_space();
	char* str1 = "0x0000001000";
	size_t* a = get_oct_size(str1);
	kprintf("size1 %d \n",sizeof(posix_header_t));

	schedule();
	*/

	while(1);
}

/*the dummy function for testing*/
void print_thread(){
	/*
	kprintf("Thread 1 \n");
	schedule();
	kprintf("Thread 2 \n");
	schedule();
	kprintf("Thread 3 \n");
	schedule();
	*/
	switch_to_ring3(current);
	while(1);
}

/*schedule function*/
void schedule(){
	context_switch(current,current->next);
}


void user_func(){
	kprintf("testing for r3\n");
	while(1);
}
