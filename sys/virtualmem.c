#include <sys/virtualmem.h>
#include <sys/physmem.h>
#include <sys/kprintf.h>

pml4_t PML4;

void init_kernalmem(unsigned long physfree){
    kprintf("print physfree !!!! %x \n",physfree);
	//kernal_base is the virtual address where actual kernal ends. The address is the start point we can used for kmalloc
	unsigned long kernal_base = (physfree>>12) + VIRT_ST;
    kprintf("print kernal_base !!!! %x \n",physfree);
    unsigned long total = PAGE_SIZE*128;
    kprintf("print total length %x \n",total);
	memset((void *) kernal_base, 0,total);
	PML4 = (pml4_t) kmalloc(KERNAL_TB,PAGE_SIZE);

}

void* kmalloc(int flag, unsigned int size){
    unsigned long res_addr = 0x0;
    switch(flag){
        case KERNAL_TB:
            res_addr = allocate_page();
        default:
            res_addr = 0x0;
    }
    void* res = &res_addr;
    return res;
}


void *memset(void *s, int ch , unsigned long n) {
    char* tmp = s;
    while(n > 0) {
        *tmp = ch;
        tmp++;
        n--;
    }
    return s;
}




