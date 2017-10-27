#include <stdio.h>

void * get_physaddr(void* virtualaddr);





void* get_physaddr(void * virtualaddr){
	printf("virtualaddr %lu\n", *(unsigned long*)virtualaddr);
	unsigned long pml4_index = *(unsigned long*)virtualaddr >> 39 & 0x1FF;
	printf("pml4_index %lu\n", pml4_index);
	unsigned long page_dir_pt_index  =  *(unsigned long*)virtualaddr >> 30 & 0x1FF;
	printf("page_dir_pt_index %lu\n", page_dir_pt_index);
	unsigned long page_dir_index  = *(unsigned long*)virtualaddr >> 21 & 0x1FF;
	printf("page_dir_index %lu\n", page_dir_index);
	unsigned long page_tb_index =  *(unsigned long*)virtualaddr >> 12 & 0x1FF;
	printf("page_tb_index %lu\n", page_tb_index);
}


int main(){
	unsigned long virtualaddr = 0x80801fffa8;
	void* virtualaddr_ptr = (unsigned long*)&virtualaddr;
	get_physaddr(virtualaddr_ptr);
}

