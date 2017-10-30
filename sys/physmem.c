#include <sys/physmem.h>


extern page_t* physical_page_start;
extern page_t* free_pg_head;


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

void init_phy(unsigned long  number, unsigned long  index, unsigned long  page_max_number){
	page_t *page_tmp;
	unsigned int  i=0;
	for(i=0;i<page_max_number;i++){
		page_tmp=physical_page_start+i;
		page_tmp->pg_index=index;
		//used pages
		if(i<=number-1){
			page_tmp->next=NULL;
			page_tmp->occup=PG_OCCU;
			page_tmp->ref_ct=-1;
		}
		//unused pages
		else{
			if(i!=page_max_number-1){
				page_tmp->ref_ct=-1;
				page_tmp->occup=PG_FREE;
				page_tmp->next=page_tmp+1;
			}
			//last unused page
			else{
				page_tmp->ref_ct=-1;
				page_tmp->occup=PG_FREE;
				page_tmp->next=NULL;
			}
		}
		index++;
	}
	free_pg_head=physical_page_start+number;		
}

int free_page(unsigned long index){
	page_t *page_tmp;
	page_t *first_free;
	page_tmp=physical_page_start+index;
	first_free=free_pg_head;
	free_pg_head=page_tmp;
	page_tmp->occup=PG_FREE;
	page_tmp->next=first_free;
	return 1;
}








