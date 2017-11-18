#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
#define PROCESS_NUM 2048
typedef struct PCB {
	uint64_t pid;
	uint64_t rsp;
	uint64_t rip;
	struct PCB* next;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
	uint64_t* kstack;
} task_struct;


task_struct* create_kthread(/* void (*fn)(int)*/void* thread);
//void create_init_kthread( void (*fn)(int));
void add_task(task_struct * task);
void init_pid();
int get_pid();
void context_switch(task_struct *me,task_struct *next);
void init_thread_fn();
task_struct* create_init_kthread();
task_struct* create_new_kthread();
void print_thread();
#endif
