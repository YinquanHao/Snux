#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>

typedef struct PCB {
	char* kstack;
	uint64_t pid;
	uint64_t rsp;
	struct PCB* next;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
} task_struct;


task_struct* create_kthread( void (*fn)(int), char* thread_name);
void add_task(task_struct * task);

#endif
