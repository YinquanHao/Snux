#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
#define PROCESS_NUM 2048
#define CODE_VMA 0
#define DATA_VMA 1
#define HEAP_VMA 2
#define STACK_VMA 3
#define USER_STACK_TOP	 0xFFFFFFFFFFFFE000UL
#define USER_STACK_LIMIT 0xFFFFFFFFFFF5EFFFUL //the lowest address of user stack 160pgs
#define HEAP_START		 0xFFFFFFFF80000000UL
#define HEAP_LIMIT       0xFFFFFFFF800A0000UL //the highest address of heap can reach 160pgs
#define VMA_VA_ST        0xFFFFFFFF800A1000UL
//for tesing 
#define heap_end  0x8000
#define stack_end 0x8000

typedef struct vma_struct vma_struct;
typedef struct mm_struct mm_struct;

typedef struct PCB {
	uint64_t pid;
	uint64_t rsp;
	uint64_t rip;
	uint64_t cr3;
	uint64_t vir_top;
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

struct file{
    uint64_t   start;       /* start address of region */
    uint64_t   pgoff;       /* offset in file or NULL */
    uint64_t   size;        /* region initialised to here */
    uint64_t   bss_size;
};



struct regs{
	uint64_t rsp,rbp,rdi,rsi,rdx,rcx,rbx,rax;
	uint64_t r15,r14,r13,r12,r11,r10,r9,r8;
	uint64_t idx,err_code;

};

enum vma_types {	
		TEXT,
		DATA,
		STACK,
		HEAP,
		NOTYPE
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
task_struct* create_user_process(char* filename);
void set_user_task_struct_mm(task_struct* task);
vma_struct* select_vma_by_type();
task_struct* get_current_task();
void user_space_allocate(uint64_t viraddr);
#endif
