#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
// end store the last struct
task_struct* end;

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
task_struct* create_kthread( void (*fn)(int)){
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
	return task;
}


/*add a task to run queue*/
void add_task(task_struct * task) {
	if(end){
		end->next = task;
    	end = task;
	}else{
		end = task;
	}
}

void context_switch(task_struct *me,task_struct *next){
	
	//set_tss_rsp(next->kernel_stack_base);
	//set_tss_rsp(next->kstack);
	__asm volatile(
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

	__asm volatile("movq %%rsp, %0"
            :"=r"(me->rsp)
    );

    __asm volatile("movq %0, %%rsp"
            ::"r"(next->rsp)
    );

    /*__asm volatile("movq %0, %%cr3;"
            ::"r"(next->cr3)
    );*/

	__asm volatile(
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

    me->state=SLEEPING;
    next->state=RUNNING;
    //kprintf("switch!");

}


void print_kthread(){
	kprintf("thread id");
}
