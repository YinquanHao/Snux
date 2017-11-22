#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
#define PROCESS_NUM 2048
#define CODE_VMA 0;
#define DATA_VMA 1;
#define HEAP_VMA 2;
#define STACK_VMA 3;

typedef struct vma_struct vma_struct;
typedef struct mm_struct mm_struct;

typedef struct PCB {
	uint64_t pid;
	uint64_t rsp;
	uint64_t rip;
	uint64_t cr3;
	struct PCB* next;
	mm_struct* mm;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
	uint64_t* kstack;
} task_struct;


struct mm_struct {
	vma_struct *mmap, *current;
	uint64_t start_code, end_code, start_data, end_data;
	uint64_t start_brk, brk, start_stack;
	uint64_t arg_start, arg_end, env_start, env_end;
	uint64_t rss, total_vm, locked_vm;
};

struct vma_struct {
	mm_struct *mm;
	uint64_t start;              // Our start address within vm_mm
	uint64_t end;                // The first byte after our end address within vm_mm
	vma_struct *next;            // linked list of VM areas per task, sorted by address
	uint64_t flags;              // Flags read, write, execute permissions
	uint64_t type;               // type of segment its refering to
	struct file *file;           // reference to file descriptors for file opened for writing
};


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
void user_func();
void switch_to_ring3();
task_struct* create_user_process();
void set_user_task_struct_mm(task_struct* task);
vma_struct* select_vma_by_type();
#endif
