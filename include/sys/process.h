#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
#define PROCESS_NUM 2048
typedef struct PCB {
	char* kstack;
	uint64_t pid;
	uint64_t rsp;
	struct PCB* next;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
} task_struct;


task_struct* create_kthread( void (*fn)(int));
void add_task(task_struct * task);
void init_pid();
int get_pid();
void context_switch(task_struct *me,task_struct *next);

#endif
