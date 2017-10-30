#include <sys/physmem.h>


//get the start of physical mem that can be used my kmalloc
unsigned long  get_kmalloc_base(unsigned long physfree, unsigned long smap_base) {
	//phyfree>>12 -> get num of pages total below physfreee
	physfree = (physfree>>12)+((physfree-smap_base)>>12);
	return physfree<<12;
}



/*
get the first free page and allocate it.
@param:
	physfree_pg_start:
		The index of first physfree page we can use in the memory
@return:
	the physical address where the first free page
*/
unsigned long allocate_page(unsigned int physfree_pg_start){
	unsigned int pg_index = get_free_pg();
	page_t* pg = physfree_pg_start  + pg_index;
	pg->occup = PG_OCCU;
	return (unsigned long)(pg->pg_index << 12);
}




/*
get first free_list
@return:
	the index of first free_pg
*/
unsigned int get_free_pg(page_t* head){
	if(head){
		unsigned int index = head->index;
		head = head->next;
		return index;
	}else{
		printf("No free page finded");
		return 0;
	}
}



