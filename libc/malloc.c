#include <stdio.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <sys/virtualmem.h>
#include <stdlib.h>
#define ALLOC 1
#define FREE 0

struct super_block* head = NULL;
void* cur_brk = NULL;


void* brk(int pgnum){
	void* res;
	//printf("syscall_1 ret %d\n", syscall_1(SYS_brk,(uint64_t)pgnum));
	//while(1);
	//rintf("uint64_t size %d", sizeof(uint64_t));
	uint64_t resa = 10;
	resa = syscall_1(SYS_brk,(uint64_t)pgnum);
	//res = 999;
	//printf("resaaa!!!! %d", resa);
	//printf("res!!!! %d", res);

	//while(1);
	return resa;
}

void* sbrk(size_t size){
	void* oldbrk;
	int pgnum = (size/PAGE_SIZE)+1;
	oldbrk = brk(pgnum);
	return oldbrk;
}

struct super_block* get_free(struct super_block** last, size_t size){
	struct super_block *new;
	new = head;
	while(new!=NULL){
		if((new->status == FREE) && (new->size >=size)){
			break;
		}
		*last = new;
		new = new->next; 
	}
	return new;
}

struct super_block* request_block(struct super_block* prev,size_t size){
	struct super_block *new_block;
	new_block = (struct super_block*) sbrk(sizeof(struct super_block)+size);
	
	if(prev!=NULL){
		prev->next = new_block;
		new_block->prev = prev;
	}
	new_block->status = ALLOC;
	new_block->size = size;
	new_block->next = NULL;
	return new_block;
}


struct super_block *get_block_header(void *pt){
    struct super_block *new;
	new = (struct super_block *)pt;
	new= new - 1;
    return new;
}

void free(void *data){
	struct super_block *new;
	if(data == NULL){
		return;
	}
	new = (struct super_block*) get_block_header(data);
	if(new->status == FREE){
		return;
	}else{
		new->status = FREE;
	}
	size_t size = new->size;
	char* tmp = data;
	while(size>0){
		*tmp = 0;
		tmp++;
		size--;
	}
}

void* malloc(size_t size){
	struct super_block *cur;
	struct super_block *prev;
	//cur = request_block(NULL, size);
	//printf("%s\n",cur->status );
	
	if(size<0) return NULL;
	if(head == NULL){
		cur = request_block(NULL,size);
		if(cur == NULL){
			return NULL;
		}
		head = cur;
		cur->prev = NULL;

	}else{
		prev = head;
		cur = get_free(prev,size);
		if(cur == NULL){
			cur = request_block(prev,size);
			if(cur == NULL){
				return NULL;
			}
		}else{
			cur->status = ALLOC;
		}
	}
	cur++;
	return((void*)cur);
}