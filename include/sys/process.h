#ifndef _PROCESS_H
#define _PROCESS_H


typedef struct PCB {
	char kstack[4096];
	uint64_t pid;
	uint64_t rsp;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
} task_struct;


#endif