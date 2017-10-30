#include <sys/physmem.h>


//get the start of physical mem that can be used my kmalloc
unsigned long  get_kmalloc_base(unsigned long physfree, unsigned long smap_base) {
	//phyfree>>12 -> get num of pages total below physfreee
	physfree = (physfree>>12)+((physfree-smap_base)>>12);
	return physfree<<12;
}