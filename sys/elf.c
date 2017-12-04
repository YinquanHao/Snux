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

	kprintf("start->name %s \n",start->name);
	//kprintf("start->name %d \n",get_oct_size(start->size));

	//initial the cur of the tarf as the start
	uint64_t *cur = (uint64_t *)start;
	//iterator through the header and find the file we want to access
	 while ((start < (posix_header_t*) &_binary_tarfs_end)&&(start->name[0]!='\0')){
	 	kprintf("start->name %s \n",start->name);
	 	//if the name of the file does not match skip the size of file
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
	 		//return the file
	 		return (void *)start;
	 	}
	 }
	return NULL;
}

//calculate the size in oct string
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

//load the elf
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

	int i=0;
	//iterator thought the number of headers in the ehdr
	for(i=0;i<ehdr->e_phnum; i++) {
		//if the p_type is loadable
		if(phdr->p_type == 1) {

			//align the p_vaddr down
			uint64_t start = phdr->p_vaddr;
			start = (start/PAGE_SIZE)*PAGE_SIZE;
			//align the p_vaddr+p_memsz up
			uint64_t end = phdr->p_vaddr+phdr->p_memsz;
			end = (end/PAGE_SIZE)*(PAGE_SIZE)+PAGE_SIZE;

			kprintf("start %x \n",start);
			kprintf("end %x \n",end);

			//get the length of the file
			uint64_t length = end - start;
			//how many pgs we need to allocate
			uint64_t pg_num = length/PAGE_SIZE;

			kprintf("length %x \n",length);
			kprintf("pg_num %x \n",pg_num);

			//allocate a new page for the vma
			user_space_allocate(task->vir_top);
			//set the vma to the virtual_addr
			vma_struct *vma = (vma_struct*)(task->vir_top);
			//increase the high water virtual addr in a user process
			task->vir_top+=PAGE_SIZE;

			uint64_t start_ct = start;
			//allocate and mapping the pgs for the p_vaddr(This is a virtual addr!)
			while(pg_num>0){
				user_space_allocate(start_ct);
				start_ct+=PAGE_SIZE;
				pg_num--;
			}

			//set the vma to mm->mmap
			if(mm->mmap == NULL) {
				mm->mmap = vma;
			} else {
				mm->current->next = vma;	
			}
			vma->mm = mm;
			mm->current = vma;

			vma->start = phdr->p_vaddr;
			vma->end = phdr->p_vaddr + phdr->p_memsz;
			vma->flags = phdr->p_flags;
			vma->file = NULL;
			vma->next = NULL;
			//check if it's data section or text section
			if((phdr->p_flags == (PERM_R | PERM_X)) || (phdr->p_flags == (PERM_R | PERM_W))){
				//set the task->mm
				task->mm->start_code = phdr->p_vaddr;
				task->mm->end_code = phdr->p_vaddr + phdr->p_memsz;
				//allocate and map a new page for the vma->file
				user_space_allocate(task->vir_top); 
				vma->file = (struct file *)task->vir_top;
				task->vir_top+=PAGE_SIZE;
				
				//set vma->file structure's start as ehdr and other attributes
				vma->file->start = (uint64_t)ehdr;
				vma->file->pgoff = phdr->p_offset;
				vma->file->size = phdr->p_filesz;

				//copy the ehdr's content into vma->start(what we have created before)
				uint64_t sum = (uint64_t)ehdr +(uint64_t)phdr->p_offset;
				kprintf("sum %d",sum);
				memcpy((void*)vma->start, (void*)(sum), phdr->p_filesz);
				//set the type and bss_size
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

	//allocate a new page for user stack
	user_space_allocate(USER_STACK_TOP-PAGE_SIZE);
	vma_struct *vma_stack = (vma_struct*)(USER_STACK_TOP-PAGE_SIZE);
	if(mm->mmap == NULL)
		mm->mmap = vma_stack;
    else
    	mm->current->next = vma_stack;
    mm->current = vma_stack;	
	uint64_t *stack = USER_STACK_TOP;	
	vma_stack->start = (uint64_t)(stack); 
	vma_stack->end = (uint64_t)(stack-PAGE_SIZE);
	vma_stack->flags = (PERM_R | PERM_W);
	vma_stack->type = STACK;
	vma_stack->file = NULL;
	vma_stack->next = NULL;
	//set task's rsp to the user_stack's addr
	task->rsp = (uint64_t)((uint64_t)stack - 16);

	//allocate a new page for user heap
	user_space_allocate(HEAP_START);
	vma_struct *vma_heap = (vma_struct*)(HEAP_START);
	if(mm->mmap == NULL)
		mm->mmap = vma_heap;
    else
    	mm->current->next = vma_heap;
    mm->current = vma_heap;	
	uint64_t *heap = HEAP_START;
	mm->brk = HEAP_START;	
	vma_heap->start = (uint64_t)(HEAP_START); 
	vma_heap->end = (uint64_t)(HEAP_START+PAGE_SIZE);
	vma_heap->flags = (PERM_R | PERM_W);
	vma_heap->type = HEAP;
	vma_heap->file = NULL;
	vma_heap->next = NULL;
	return 0;
}