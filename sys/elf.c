#include <sys/elf.h>
#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/virtualmem.h>

//load binary files to tarf space
void* get_binary(char *filename){
	//if no filename return NULL
	if(filename==NULL) return NULL;
	//get the posix_header_ustar points to the addr of _binary_tarfs_start
	posix_header_t *start = (posix_header_t*)&_binary_tarfs_start;

	//kprintf("start->name %s \n",start->name);
	//kprintf("start->name %d \n",get_oct_size(start->size));

	//while(1);
	//initial the cur of the tarf as the start
	uint64_t *cur = (uint64_t *)start;

	 while ((start < (posix_header_t*) &_binary_tarfs_end)&&(start->name[0]!='\0')){
	 	kprintf("start->name %s \n",start->name);
	 	if(strcmp(start->name,filename)!=0){
	 		int size = get_oct_size(start->size);
	 		if(size==-1){
	 			break;
	 		}
	 		if(size%512){
				size /= 512;
				size *= 512;
				size += 512;   
			}
			start = (posix_header_t *)((uint64_t)start + size + sizeof(posix_header_t));  
			cur = (uint64_t *)start; 
	 	}else{
	 		return (void *)start;
	 	}
	 }
	return NULL;
}

int get_oct_size(char* str){
	int i;
	int res = 0;
	int muti = 1;
	for(i=10;i>1;i--){
		if(str[i]<'0'&&str[i]>'9'){
			return -1;
		}
		res = res+(str[i]-'0')*muti;
		muti*=8;
	}
	return res;
}


int load_elf(task_struct* task, void* exe){

	mm_struct *mm = task->mm;
	//get elf header
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)exe;
	//get ehdr header
	Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64_t)ehdr + ehdr->e_phoff);
	//set the initial mm->mmap to NULL
	task->mm->mmap = NULL;
	//assign the task's rip
	task->rip = ehdr->e_entry;
/*
	int i=0;

	for(i=0;i<ehdr->e_phnum; i++) {

		if(phdr->p_type == 1) {

			//vma_struct *vma = (vma_struct *)kmalloc();

			user_space_allocate(VIRT_ST|0x5002000);
			vma_struct *vma = (vma_struct*)(VIRT_ST|0x5002000);

			if(mm->mmap == NULL) {
				mm->mmap = vma;
			} else {
				mm->current->next = vma;	
			}
			vma->mm = mm;
			mm->mmap->current = vma;

			vma->start = phdr->p_vaddr;
			vma->end = phdr->p_vaddr + phdr->p_memsz;

			uint64_t st = ((phdr->p_vaddr/PAGE_SIZE)*PAGE_SIZE);
			uint64_t ed = ((phdr->p_vaddr/PAGE_SIZE)*PAGE_SIZE);
			uint64_t length  = ed -st;
			uint64_t pg_num = length/PAGE_SIZE;
			while(pg_num){
				//allocate pg
				//cp start to pg
				user_space_allocate(VIRT_ST|0x5003000);
				//vma_struct *vma = (vma_struct*)(VIRT_ST|0x5002000);
				memcpy((void*)VIRT_ST,st,PAGE_SIZE);
				length--;
				st+=PAGE_SIZE;
			}
			vma->flags = phdr->p_flags;
			vma->file = NULL;
			vma->next = NULL;


			if((phdr->p_flags == (PERM_R | PERM_X)) || (phdr->p_flags == (PERM_R | PERM_W))){

				task->mm->start_code = phdr->p_vaddr;
				task->mm->end_code = phdr->p_vaddr + phdr->p_memsz;
				vma->file = (struct file *)kmalloc();  
				vma->file->start = (uint64_t)ehdr;
				vma->file->pgoff = phdr->p_offset;
				vma->file->size = phdr->p_filesz;	
				
				memcpy((void*)vma->start, (void*)((uint64_t)ehdr + phdr->p_offset), phdr->p_filesz);
				if(phdr->p_flags == (PERM_R | PERM_X)) {
					vma->file->bss_size = 0;
					vma->type = TEXT;
				} else {
					vma->file->bss_size = phdr->p_memsz - phdr->p_filesz;
					vma->type = DATA; 
				}	
			}
		}
		phdr++;
	}

	// Allocate HEAP  and map to HEAP_ST 
	vma_struct *vma_heap = (vma_struct *)kmalloc();

	if(mm->mmap == NULL)
		mm->mmap = vma_heap;		
	else
		mm->current->next = vma_heap;

	mm->current = vma_heap;
	vma_heap->mm = mm; 
	vma_heap->start = HEAP_START;
	mm->brk = HEAP_START;
    vma_heap->end = HEAP_START + PAGE_SIZE;
    vma_heap->flags = (PERM_R | PERM_W);
    vma_heap->type = HEAP;
    vma_heap->file = NULL;
    vma_heap->next = NULL;
	
    // Allocate STACK  and map to STACK_ST 
	vma_struct *vma_stack = (vma_struct *)kmalloc();

	if(mm->mmap == NULL)
		mm->mmap = vma_stack;
    else
    	mm->current->next = vma_stack;
    mm->current = vma_stack;	

	uint64_t *stack = USER_STACK_TOP;	
	vma_stack->start = stack + PAGE_SIZE; 
	vma_stack->end = stack;
	vma_stack->flags = (PERM_R | PERM_W);
	vma_stack->type = STACK;
	vma_stack->file = NULL;
	vma_stack->next = NULL;

	task->rsp = (uint64_t)((uint64_t)stack + 4096 - 16);
*/
	//mm_struct *mm = task->mm;
	
	user_space_allocate(USER_STACK_TOP-PAGE_SIZE);
	vma_struct *vma_stack = (vma_struct*)(USER_STACK_TOP-PAGE_SIZE);
	if(mm->mmap == NULL)
		mm->mmap = vma_stack;
    else
    	mm->current->next = vma_stack;
    mm->current = vma_stack;	
	uint64_t *stack = USER_STACK_TOP;	
	vma_stack->start = (uint64_t)(stack-PAGE_SIZE); 
	vma_stack->end = (uint64_t)stack;
	vma_stack->flags = (PERM_R | PERM_W);
	vma_stack->type = STACK;
	vma_stack->file = NULL;
	vma_stack->next = NULL;
	task->rsp = (uint64_t)((uint64_t)stack - 16);


	user_space_allocate(HEAP_START);
	vma_struct *vma_heap = (vma_struct*)(HEAP_START);
	if(mm->mmap == NULL)
		mm->mmap = vma_heap;
    else
    	mm->current->next = vma_heap;
    mm->current = vma_heap;	
	uint64_t *heap = HEAP_START;	
	vma_heap->start = (uint64_t)(HEAP_START); 
	vma_heap->end = (uint64_t)(HEAP_START+PAGE_SIZE);
	vma_heap->flags = (PERM_R | PERM_W);
	vma_heap->type = HEAP;
	vma_heap->file = NULL;
	vma_heap->next = NULL;
	//task->rsp = (uint64_t)((uint64_t)stack - 16);


	return 0;

	
}


/*
int oct2bin(unsigned char *str, int size){
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}
*/