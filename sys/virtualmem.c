#include <sys/virtualmem.h>

void init_kernalmem(unsigned long physfree){
	//kernal_base is the virtual address where actual kernal ends. The address is the start point we can used for kmalloc
	kernal_base = (physfree>>12) + VIRT_ST;
	memset((void *) kernpt_base, 0, KERNAL_PG_TB_NUM * PAGE_SIZE);
	//PML4 = (pml4_t) kmalloc();
}



void *memset(void *s, int ch , unsigned int n) {
    char* tmp = s;
    while(n > 0) {
        *tmp = ch;
        tmp++;
        n--;
    }
    return s;
}
