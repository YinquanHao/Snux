#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/terminal.h>
#include <sys/virtualmem.h>
extern task_struct* current;

void set_msr(){
	uint64_t msr=0xC0000080;
	uint32_t low_32;
	uint32_t high_32;
	//set EFER SCE
	__asm__ __volatile__(
		"rdmsr;"
		"orq $0x1,%%rax;"
		"wrmsr;"
		::"c"(msr)
	);
	//set STAR
	msr=0xC0000081;
	high_32=(0x1b<<16)|(0x8);
	low_32=0x0;
	__asm__ __volatile__("wrmsr;" 
		::"c" (msr), "a" (low_32), "d" (high_32)
	);
	//set LSTAR
	msr=0xC0000082;
	uint64_t syscallrip=(uint64_t)syscall_entry;//&syscall_handler;
	low_32=(uint32_t)syscallrip;
	high_32=(uint32_t)(syscallrip>>32);
	__asm__ __volatile__("wrmsr;" 
		::"c" (msr), "a" (low_32), "d" (high_32)
	);
	//set CSTAR
	msr=0xC0000083;
	uint64_t rip=&syscall_compatible_mode;
	low_32=(uint32_t)rip;
	high_32=(uint32_t)(rip>>32);
	__asm__ __volatile__("wrmsr;" 
		::"c" (msr), "a" (low_32), "d" (high_32)
	);
	//SFMASK
	msr=0xC0000084;
	low_32=0x0;
	high_32=0x0;
	__asm__ __volatile__("wrmsr;" 
		::"c" (msr), "a" (low_32), "d" (high_32)
	);
	
/*	msr=0xC0000102;
	low_32=0x5000;
	high_32=0x200;
	__asm__ __volatile__("wrmsr;" 
		::"c" (msr), "a" (low_32), "d" (high_32)
	);*/

/*uint64_t res;
	__asm__ __volatile__(
		"rdmsr;"
		::"c"(msr)
	);
		__asm__ __volatile__(
		"movq %%rax, %0;"
		:"=r"(res)
	);
	kprintf("%x",res);
	while(1);*/
}


uint64_t syscall_handler(struct syscall_regs* regs){
	
//	__asm__ __volatile__("xchg %bx, %bx");
//kprintf("%x, %x",regs->rpointer,regs->flags);
//while(1);
	switch(regs->rax){
		case SYS_exit:
			kprintf("call exit");
			break;
		case SYS_fork:
			break;
		case SYS_getpid:
			getpid(regs);
			break;
		case SYS_getcwd:
			break;
		case SYS_chdir:
			break;
		case SYS_write:
			sys_write(regs);
			break;
		case SYS_read:
			break;
		case SYS_open:
			break;
		case SYS_close:
			break;
		case SYS_pipe:
			break;
		case SYS_execve:
			break;
		case SYS_mmap:
			break;
		case SYS_getdents:
			break;
//		case SYS_mumap:
//			break;
		case SYS_wait4:
			break;
		case SYS_brk:
			return sys_brk(regs);
			break;
	}
	//__asm__ __volatile__("xchg %bx, %bx");
//while(1);
	return;




/*syscall number in rax*/
/*	__asm__ __volatile__("cli;");
	__asm__ __volatile__("xchg %bx, %bx");
	uint64_t kernel_rsp=(uint64_t)current->kstack;
	uint64_t syscall_no;
	uint64_t user_rsp=0x1000;
	//kprintf("%x",kernel_rsp);
	__asm__ __volatile__(
		"movq %0,%%r10;"
		::"d"(user_rsp)
	);
	while(1);
	__asm__ __volatile__(
		"movq %%rax, %0;"
		:"=r"(syscall_no)
	);

	__asm__ __volatile__(
		"movq %%rsp, %0;"
		:"=a"(user_rsp)
	);

	__asm__ __volatile__(
		"movq %0, %%rsp;"
		:"=a"(current->kstack)
	);*/

	//kprintf("%x ,%x",current->kstack,user_rsp);


	//__asm__ __volatile__("xchg %bx, %bx");

/*	__asm__ __volatile__(
		"swapgs;"
		//"movq %gs:0,%rsp;"
	);
	//back up the current rsp
	__asm__ __volatile__(
		"swapgs;"
		"movq %rsp, %gs"
	);


	__asm__ __volatile__(
		"pushq %r15;"
		"pushq %r14;"
		"pushq %r13;"
		"pushq %r12;"
		"pushq %rbx;"
		"pushq %rbp;"
		"pushq %rdi;"
		"pushq %rsi;"
	);

	__asm__ __volatile__(
		"pushq %rcx;"
		"pushq %r11;"
	);*/

	//uint64_t syscall_no=regs->rax;

//while(1);
/*	__asm__ __volatile__(
		"popq %r11;"
		"popq %rcx;"
	);

	__asm__ __volatile__(
		"popq %rsi;"
		"popq %rdi;"
		"popq %rbp;"
		"popq %rbx;"
		"popq %r12;"
		"popq %r13;"
		"popq %r14;"
		"popq %r15;"
	);*/



//kprintf("%x",user_rsp);
/*	__asm__ __volatile__(
		"movq %0, %%rsp;"
		:"=a"(user_rsp)
	);
*/
//while(1);

	//__asm__ __volatile__("xchg %bx, %bx");


}

void getpid(struct syscall_regs* regs){
	regs->rax=current->pid;
	return;
}

uint64_t sys_write(struct syscall_regs* regs){
	terminal_write(regs->rdi,regs->rsi,regs->rdx);
}


void syscall_compatible_mode(){
	return;
}

//the sys_brk syscall to increment the brk
uint64_t sys_brk(struct syscall_regs* regs){
	int pages = regs->rdi;
	//get current task's heap 
	uint64_t addr = current->mm->brk;
    //struct vm_struct *vma_ptr;
    struct mm_struct *mm_ptr = current->mm;
    uint64_t size;
    size = pages*PAGE_SIZE;
    mm_ptr->brk +=size;
    mm_ptr->end_data +=size;
    mm_ptr->total_vm   +=size;
    kprintf("addr %x", addr);
    regs->rax = addr;
    //kprintf('addr1 %x',regs->rax)
    //while(1);
    //TODO yinquanhao do we need to actually allocate a space?
    return addr;
}