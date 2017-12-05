#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/terminal.h>
#include <sys/virtualmem.h>
#include <dirent.h>
#include <sys/string.h>
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
			regs->rax = sys_read((uint64_t)regs->rdi,(uint64_t)regs->rsi,(uint64_t)regs->rdx);
			break;
		case SYS_open:
			regs->rax = sys_open((char*)regs->rdi,(uint64_t)regs->rsi);
			break;
		case SYS_close:
			regs->rax = sys_close((uint64_t)regs->rdi);
			break;
		case SYS_pipe:
			break;
		case SYS_execve:
			break;
		case SYS_mmap:
			break;
		case SYS_getdents:
			regs->rax = sys_getdents((uint64_t)regs->rdi,(uint64_t)regs->rsi,(uint64_t)regs->rdx);
			break;
//		case SYS_mumap:
//			break;
		case SYS_wait4:
			break;
		case SYS_brk:
			return sys_brk(regs);
			break;
		case SYS_opendir:
			regs->rax = (uint64_t)sys_opendir((char*)(regs->rdi));
			break;
		case SYS_readdir:
            regs->rax = (uint64_t)sys_readdir((struct DIR*)(regs->rdi));
            break;
        case SYS_closedir:
            regs->rax = (uint64_t)sys_closedir((struct DIR*)(regs->rdi));
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

pid_t getpid(struct syscall_regs* regs){
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
    return addr;
}



//open an dirent
DIR* sys_opendir(char *path){
	char* name = get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    file_t *node = root;
    file_t *node_temp;
    //copy path name to the dirent
    strcpy(name,path);
    name = strtok(name,"/");
    kprintf("path  %s \n",name);
	if (name != NULL) {
		// if the name is either "." or ".." means stay on root dirent
		if ((strcmp(name,".") == 0)||(strcmp(name,"..") == 0)){ 
			//set the node at the current dirent of the user task
            node = current->cur_dir;
        }
	}
    while ( name !=NULL ) {
    	//set temp_node as node 
    	node_temp = node;
    	//go to self
    	if (strcmp(name,".") == 0 ) {
    		node = node->child[0];
        //go to parent
        } else if (strcmp(name,"..") == 0) {
        	node = node->child[1];
        }else {
        	int i;
        	for (i=2; i < node->last ; i++) {
                if (strcmp(name,node->child[i]->name) == 0) {
                    node = node->child[i];
                    break;
                }
            }
            //get the last file in this level return NULL
            if (i == node_temp->last) {
            	return (DIR *)NULL;
            }
        }
        name = strtok(NULL,"/");
    }
    if (node->type == DIRECTORY) {
    	//kprintf("enter this");
    	DIR* res = (DIR *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    	res->node = node;
    	res->current = 2;
    	//kprintf("res->current %x \n",res->current);
        return res;
    }
    //it's a file not a directory
    else{
    	return (DIR *)NULL;
    }
}

//read an dirent
dirent* sys_readdir(struct DIR* dir){
	if ((dir->current > 0) && (dir->node->last > 2) && (dir->current < dir->node->last)){
        strcpy(dir->current_dirent.d_name,dir->node->child[dir->current]->name);
        dir->current++;
        return &dir->current_dirent;
    }else{
    	return NULL;
    }
}

/* close a direct*/
int sys_closedir(struct DIR* dir){
	if (dir->current>1 && dir->node->type == DIRECTORY ) {
		dir->node =(file_t *)NULL;
        dir->current = 0;
        return 0;
    }else{
    	return -1;
    }
}



int sys_open(char *path, uint64_t flag ){
    file_t *node;
    file_t *node_temp;
    char *name;
    char *dirpath;
    int i=0;
    int count = 2;
    struct fd *fd1 = (struct fd*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    node = root;
	dirpath = (char *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));;
    strcpy(dirpath,path);
    name = strtok(dirpath,"/"); /* get the first token */
    if (name == NULL){
    	return -1;
    }
    if (strcmp(name, "rootfs") == 0) {
        while ( name !=NULL ) {
        node_temp = node;
        if (strcmp(name,".") == 0 ) { /* logic to detect dot notation in addresing */
            node = node->child[0];
        }else if (strcmp(name,"..") == 0) {
            node = node->child[1];
        }else{
        	for (i=2; i < node->last ; i++) {
        		if (strcmp(name,node->child[i]->name) == 0) {
                    node = (file_t *)node->child[i];
                                break;
                            }
                        }
                    }
                    if (i >= node_temp->last)
                        return -1;
                    name = strtok(NULL,"/");
                }
                if ((node->type == DIRECTORY && flag == (O_RDONLY)) || (node->type == FILE)){
                    fd1->node = node;
                    fd1->permission =flag;
                    fd1->current = node->first;
                } else {
                        return -1;
                }
                while ((current->fd[++count] != NULL) && count < 100);
                if (count >= 100)
                        return -1;
                else{
                	current->fd[count] = fd1;
                	return count;
                }
        }
    return -1;
}



/* syscall to read from file */
int sys_read(uint64_t fd_count,uint64_t addr,uint64_t len)
{

        uint64_t len_read=0;
        uint64_t len_end =0;
        uint64_t count;

//      struct task_struct *current_task = current;
        /*
        if (fd_count == stdin) {
            count = scanf_stdin((void *)addr,len);//define it
             return count;
        }
		*/
       if ((current->fd[fd_count] != NULL) && (current->fd[fd_count]->permission != O_WRONLY)) {
         len_read = current->fd[fd_count]->current;
         len_end  = current->fd[fd_count]->node->last;
        if (len > (len_end - len_read))
                len = len_end - len_read;
         current->fd[fd_count]->current +=len;
         memcpy((void *)addr,(void *)len_read,len);//define memcpy

       return len;
      }

     return -1;
}


//close a file descriptor
int sys_close(int fd){
	if(current->fd[fd]==NULL) return -1;
	current->fd[fd] = NULL;
	return 0;
}


//get dirent
uint64_t sys_getdents(int fd,uint64_t buf,uint64_t len){
    dirent *dir = (dirent *)buf;
    dir->inode_no = current->fd[fd]->inode_no;
    memcpy(dir->d_name,current->fd[fd]->node->name,len);
	return (uint64_t)len;
}









/*
char *print_node(file_t *p_node){
        char node_buf[10][30];
//      char buffer[100];
        int index = 0;
        int buf_i = 0;
        char *str = buffer;
        bzero(node_buf,300);

        file_t *node = p_node;
        if (node == NULL)
                return (char *)NULL;
        while (node != root) {
        strcpy(node_buf[index],node->name);
        index++;
        node = node->child[1];
        }

        while (index >=0)
        {
                while( node_buf[index][buf_i] != '\0')
                {
                        *str = node_buf[index][buf_i];
                        buf_i++;
                        str++;
                }

                buf_i = 0;
                index--;
                if (index >= 0)
                        *str++ = '/';
        }
        str++;
        *str = '\0';
        return buffer;
}*/