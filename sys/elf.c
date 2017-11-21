#include <sys/elf.h>
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/virtualmem.h>

//load binary files to tarf space
void* get_binary(char *filename){
	//if no filename return NULL
	if(filename==NULL) return NULL;
	//get the posix_header_ustar points to the addr of _binary_tarfs_start
	posix_header_t *start = (posix_header_t*)&_binary_tarfs_start;
	//initial the cur of the tarf as the start
	uint64_t *cur = (uint64_t *)start;

	 while (start < (posix_header_t*) &_binary_tarfs_end) {
	 	if(strcmp(start->name,filename)!=0){
	 		size_t size = get_oct_size(start->size);
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

size_t get_oct_size(char* str){
	int i;
	int res = 0;
	int muti = 1;
	for(i=11;i>1;i--){
		res = res+(str[i]-'0')*muti;
		muti*=8;
	}
	return res;
}


int load_elf(){
	
}