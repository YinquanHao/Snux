#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/terminal.h>
#include <sys/virtualmem.h>
#include <dirent.h>
#include <sys/string.h>
#include <sys/tarfs.h>
extern task_struct* current;
extern task_struct* end;

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
}


uint64_t syscall_handler(struct syscall_regs* regs){
	switch(regs->rax){
		case SYS_exit:
			sys_exit((uint64_t)regs->rdi);
			break;
		case SYS_fork:
			regs->rax = sys_fork(regs);
			return regs->rax;
			break;
		case SYS_getpid:
			getpid(regs);
			break;
		case SYS_getcwd:
			regs->rax = sys_getcwd((char*)regs->rdi,(uint64_t)regs->rsi);
			break;
		case SYS_chdir:
		    regs->rax = sys_chdir((char*)regs->rdi);
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
			regs->rax  = sys_execve((uint64_t)regs->rdi,(uint64_t)regs->rsi,(uint64_t)regs->rdx);
			break;
		case SYS_mmap:
			break;
		case SYS_listfiles:
		 	regs->rax = sys_listfiles((char *)regs->rdi);
		 	break;
		case SYS_getdents:
			regs->rax = sys_getdents((uint64_t)regs->rdi,(uint64_t)regs->rsi,(uint64_t)regs->rdx);
			break;
//		case SYS_mumap:
//			break;
		case SYS_wait4:
			syscall_wait4(regs);
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
        case SYS_nanosleep:
            sys_sleep((uint64_t)regs->rdi);
            regs->rax=0;
            break;
        case SYS_ps:
            task_list();
            break;
	}
	return;
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
        //uint64_t count;

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
         memcpy((void *)addr,(void *)len_read,len);

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


// change process's current working directory
int sys_chdir(char *path){
    DIR *newdir = sys_opendir(path);
	if (newdir == NULL)
		return -1;
	current->cur_dir = newdir;
	strcpy(current->cwd,path);
	return 0;
}

//get current working directory
uint64_t sys_getcwd(char *buf, size_t size){
	if(size>=strlen(current->cwd)){
		strcpy(buf,current->cwd);
		return (uint64_t)buf;
	}
	return NULL;
}




//execve function
//TODO YinquanHao need to enable the argv and envp functionality
int sys_execve(char *filename, char **argv, char **envp){
	//create a buffer for the argv and envp
	char buf[6][128];
	//memset


	memset(buf,'\0',6*128);
	/* push the filename on the userstack */
	uint64_t argc=0; 
	strcpy(buf[argc], filename);
	argc++;
	/* if arguments passed are not NULL copy them */ 
	if(argv != NULL) {
		uint64_t ct = 0;
		while(argv[ct] != NULL){
            strcpy(buf[argc],argv[ct]);
            argc++;
			ct++;
        }
	}
    	  
	/* create a new task structure */
	task_struct *task = (task_struct*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));

	/* copy pid and ppid */
	task->pid = current->pid;
	task->ppid = current->ppid;

	/* copy file descriptors */ 
	memcpy(&(task->fd[0]),&(current->fd[0]),(sizeof(current->fd[0])* 100));

    /* read the elf header from the provided binary */
    posix_header_t *hd = get_binary(buf[0]);

    if(hd == NULL) {
        kprintf("No Such Binary\n");
        return -1;
    }

    //get the prev_cr3 and store it in prev_cr3
    void* prev_cr3 = get_CR3();
    //create new pml4 and this pml4 is in kernel space
	void* new_PML4 = set_user_addr_space();
    //mask the new_PML4's address back to physical
    new_PML4 = ((uint64_t)new_PML4)^VIRT_ST;
    //set the task->cr3 to new created PML4 (physical)
    task->cr3 = new_PML4;
    //set the cr3 to new PML4
    set_CR3((uint64_t)new_PML4);

    void *stack = (void *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
	//make task->rsp points to the highest address of the stack
	task->rsp = task->kstack = ((uint64_t)stack +0x1000 -16);
    /* allocate space to mm structure */
    task->vir_top = VMA_VA_ST;
    //allocate the user_space and map it into vir_top
    user_space_allocate(task->vir_top);
    //make the mm struct points to the  mm
    mm_struct *mm = (mm_struct *)(task->vir_top);
    //increment the vir_top as one pg size
    task->vir_top+=PAGE_SIZE;
    //set task
	task->mm = mm;
	//load the elf
    load_elf(task,(void*)(hd+1));
    //TODO need to varify following part
	void *st_ptr = (void *)(USER_STACK_TOP - sizeof(buf)- 16);

    memcpy(st_ptr, (void *)buf, sizeof(buf));

	int i=argc;

    for(; i>1; i--){
    	*(uint64_t*)((uint64_t)st_ptr - (uint64_t)8*i) = (uint64_t)st_ptr + (uint64_t)(argc-i)*128;
    }

    //store the last variable as NULL on the stack
    *(uint64_t*)((uint64_t)st_ptr - 8) = NULL;


    st_ptr = st_ptr - 8*(argc);
    task->rsp = (uint64_t)st_ptr;
    //set cr3 back
	set_CR3((struct PML4 *)prev_cr3);

	/*set the new task as parents next ,set current's next as task's newxtr*/
	task_struct *prev = current->next;
	while(prev->next != current){
		prev = prev->next;
	}
	prev->next = task;
	task->next = current->next;
	current = task;
	//need to setup the end task reference
	end = task;
	/* set current directory of the process */
    strcpy(task->cwd,"/rootfs/bin");
    //switch to ring3 and execute the task
	switch_to_ring3(task);

    return -1;

}

//NOT USED?
void exit(int exit_status){
	return 0;
}

// for sys_call wrapper
uint64_t sys_fork(struct syscall_regs* regs){
	regs->rax=fork();
	return regs->rax;
}

uint64_t syscall_wait4(struct syscall_regs* regs){
	sys_dowait4(regs->rdi,regs->rsi,regs->rdx);
	return 0;
}

//listfile syscall wrapper
int sys_listfiles(char *path) {
    char a[100] = { 0 };
    strcpy(a,path);
    DIR *c = sys_opendir(a);
    dirent *dir;
    while ((dir = sys_readdir(c)) != NULL){
       kprintf("%s  ",dir->d_name);
       // sys_write(1,dir->d_name,strlen(dir->d_name));
    }
    return 0;
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

/*
int sys_catfiles(char *filename,int pipe){
	// if the filename == NULL return -1;
	if(filename == NULL)
		return -1;
	//create a buffer
	char buf[128] = {0};

    if (pipe == RD_PIPE){
        //printf("%s \n",pipe_buffer);
	print_catfiles(pipe_buffer);
    } else if (pipe == WR_PIPE || pipe == NO_PIPE){
        bzero(pipe_buffer,4096);
	int fd1;
	fd1 = sys_open(a,O_RDWR);
	if (fd1 != -1)
		sys_read((uint64_t)fd1,(uint64_t)pipe_buffer,4096);
	else { 

		dir *file = sys_opendir(path);
		if (file) {
			printf("cat : %s is a directory \n",path);
		} else {
			printf("incorrect file name \n");
		}
		return 0;
	}
	if (pipe == NO_PIPE){
		print_catfiles(pipe_buffer);
		printf("\n");	
		//printf("%s \n",pipe_buffer);
	}
    }
    return 0;
}
*/

//print by line by tokenize the input string by "/n"
void print_by_line(char* input){
	char *tok;
	tok = strtok(input,"\n");
	while (input != NULL){
		kprintf("%s \n",tok);
		tok = strtok(NULL,"\n");
	}
}