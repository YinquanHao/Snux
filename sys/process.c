#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
// end store the last struct
task_struct* end;
task_struct* first;
task_struct* current;
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


/*create a new kernel thread*/
task_struct* create_kthread( /*void (*fn)(int)*/void* thread){
	//kmalloc a task_struct for new kthread
	task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//double malloc the function
	//kmalloc(KERNAL_MEM,1);
	//assign pid for the task

	void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);

	task->pid = get_pid();
	//make rsp points to high address of kstack
	//task->rsp = (uint64_t)((task->kstack)+0x1000);
	
	//kprintf("address of kstack %x \n", task->kstack);
	//kprintf("address of rsp %x \n", task->rsp);
	//kprintf("sizeof %x \n", sizeof(task_struct));
	
	//make rip points to the function passed into thread
	task->rip = (uint64_t) & func_a;
	//state and exit_status for the task (Need to modify later not correct)
	task->state = 1;
	task->exit_status = 0;
	add_task(task);


	kprintf("the old rsp %x\n", task->kstack);
	kprintf("the old rs1 %x\n", task->rsp);
	kprintf("the rip %x\n", task->rip);

	//make the rsp points to the stack pointer in the PCB
	__asm__ __volatile__ (	
		"movq %0, %%rsp"
		::"r"(task->rsp)
	);


/*
	uint64_t a;
	__asm__ __volatile__ (
		"movq %%rsp,%0"
		:"=r"(a)
	);

	kprintf("a val %x\n", a);

*/

	//save the task->rip to register rdi
	__asm__ __volatile__(
		"movq %0, %%rax;"\
		"pushq %%rax;"\
		::"r"(task->rip)
	);

/*
	uint64_t b;
	__asm__ __volatile__ (
		"movq %%rax,%0"
		:"=r"(b)
	);

	kprintf("b val %x\n", b);
*/

	//push rdi to stack
	/*__asm__ __volatile__(
		"pushq %rax;"
	);*/

	///while(1);

	///kprintf("the new rsp %x\n", task->kstack);

	///while(1);

	__asm__ __volatile__ (
		"ret"
	);


	while(1);
	return task;


	/*
	task->pid = get_pid();
	void* stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	task->kstack = ((uint64_t)stack +0x1000-16);
	task->state = 1;
	task->exit_status = 0;
	//task->rip = (uint64_t)&fn;
	task->rip=(uint64_t)&thread;

	//make to stack pointer points to last element in the kstack
	task->rsp = ((uint64_t)stack +0x1000-16);
	add_task(task);
	return task;
	*/

	//while(1);
}

/*
void create_init_kthread( void (*fn)(int)){
	//kmalloc a task_struct for new kthread
	task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//assign pid for the task
	task->pid = get_pid();
	task->kstack = (uint64_t)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	task->state = 1;
	task->exit_status = 0;
	task->rip = (uint64_t) fn;
	//make to stack pointer points to last element in the kstack
	task->rsp = task->kstack + 4096;
	add_task(task);
	current=task;
}
*/



/*add a task to run queue*/
void add_task(task_struct * task) {
	/*the case of the task is initial task*/
	if (first == NULL){
		//kprintf("enter this\n");
		first=task;
		current=task;
		end=task;
		first->next=end;
		//kprintf("fist rsp!: %x \n",first->rsp);
	}
	/*normal case for adding task*/
	else{
		end->next=task;
		end=task;
		//kprintf("fist rsp: %x \n",first->rsp);
	}
}

void context_switch(task_struct *me,task_struct *next){

	//kprintf("kstack top %x",me->kstack[4095]);
	//kprintf("kstack top %x",me->kstack[4094]);
	//kprintf("kstack top %x",me->kstack[4096]);





	__asm__ __volatile__ (	
		"movq %%rsp, (%0)"
		::"r"((me->rsp))
	);

	//kprintf("kstack top %x",me->kstack[4095]);
	//kprintf("kstack top %x",me->kstack[4094]);
	//kprintf("kstack top %x",me->kstack[4096]);

	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		::"r"(next->rsp)
	
	);



	//while(1);


	__asm__ __volatile__ (
		"ret;"
	);
	//kprintf("current rsp: %x \n",current->rsp);

















	kprintf("me rsp!: %x \n",(me->rsp));
	kprintf("next rsp!: %x \n",(next->rsp));
	//set_tss_rsp(next->kernel_stack_base);
	//set_tss_rsp(next->rsp2);
	/*__asm__ __volatile__ (
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
	);*/

	//kprintf("current rsp1: %x \n",&(me->rsp));
	//kprintf("current kstack1: %x \n",(me->kstack));
	//kprintf("current kstack1: %x \n",(me->kstack)-1);
	/*
	__asm__ __volatile__ (
		"sti;"	
		"movq %%rsp, (%0)"
		::"r"(&(me->rsp))
	);
*/

	//kprintf("current rsp: %x \n",(me->rsp));
	//kprintf("current kstack: %x \n",me->kstack);
	//kprintf("next rsp before: %x \n",(next->rsp));

	/* move the stack pointer of current process to rsp register */ 
	/*
	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		::"r"(next->rsp)
	
	);

	kprintf("next rsp: %x \n",(next->rsp));
	kprintf("next kstack: %x \n",next->kstack);

	while(1);
*/

    /*__asm volatile("movq %0, %%cr3;"
            ::"r"(next->cr3)
    );*/

	/*__asm__ __volatile__ (
		"popq %r15;"
		"popq %r14;"
		"popq %r13;"2
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
	);*/

/*
    __asm__ __volatile__ (
    	"movq $1f, %0;"
		"pushq %1;"
		"ret;"
		"1:\t"
		:"=g"(me->rip)
		:"r"(next->rip)
    );
*/

    //me->state=SLEEPING;
    //next->state=RUNNING;
    //kprintf("switch!");


    /*__asm volatile(
		"movq $1f, %0;"
		"pushq %1;"
		"ret;"
		"1:\t"
		:"=g"(me->rip)
		:"r"(next->rip)
	);*/
	while(1);



}


void print_kthread(){
	while(1){
		kprintf("thread id");
	}
}


void func_a() {
	//while(1){
		kprintf("123213123");
		while(1);
	//}
}


void schedule(){
	//kprintf("schedule \n");
	//task_struct *prev =  first;
	//kprintf("prev1 rsp: %x \n",first->rsp);
	current = first->next;
	//kprintf("current pid: %d \n",current->pid);
	//kprintf("current kstack: %x \n",current->kstack);
	//kprintf("current rsp: %x \n",current->rsp);
	//if(current != prev){
		context_switch(first,first->next);

	//}
}
