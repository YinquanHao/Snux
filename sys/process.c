#include <sys/process.h>
#include <sys/virtualmem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
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
