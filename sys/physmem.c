#include <sys/virtualmem.h>
#include <sys/physmem.h>
#include <sys/kprintf.h>

 
extern page_t* physical_page_start;
extern page_t* free_pg_head;
/*
get the first free page and allocate it.
@return:
	the physical address where the first free page
*/
unsigned long allocate_page(){
	unsigned int pg_index = get_free_pg(free_pg_head);
	//kprintf("pg_index in allocate_page %d \n",pg_index);
	//kprintf("pg_index in allocate_page %d \n",pg_index);
	page_t* pg = (page_t*)(physical_page_start ) + pg_index;
	//kprintf("pg_index new 1 : %d \n", pg->pg_index);
	pg->occup = PG_OCCU;

	//kprintf("page index %d",pg_index);
	return (unsigned long)(pg_index)*PAGE_SIZE;
}




/*
get first free_list
@return:
	the index of first free_pg
*/
unsigned int get_free_pg(){
	if(free_pg_head){
		unsigned int index = free_pg_head->pg_index;
		free_pg_head = free_pg_head->next;
		//kprintf("head index new in get_free_pg %d", free_pg_head->pg_index);
		return index;
	}else{
		//kprintf("No free page finded");
		return 0;
	}
}


//param (number: 780, index: 256, page_max_number: 24284)
/*initialize a linkedlist like structure to store all page information start from physfree*/
void init_phy(unsigned long  number, unsigned long  index, unsigned long  page_max_number){
	//size of page_t struct = 32 bytes
	page_t *page_tmp;
	//kprintf("sizeof(page_t) %d",sizeof(page_t));
	unsigned int  i=0;

	//create a linked_list liked structure
	for(i=0;i<page_max_number;i++){
		//increment a structure size
		page_tmp=physical_page_start+i;
		//set the pg_index of the structure
		page_tmp->pg_index=index;
		//used pages where we marked as used pages 780 in our case
		if(i<=number-1){
			//set next free to null
			page_tmp->next=NULL;
			//set occupied as true
			page_tmp->occup=PG_OCCU;
			//set the count of ref as -1
			page_tmp->ref_ct=-1;
		}
		//unused pages
		else{
			//before last unused page
			if(i!=page_max_number-1){
				//set the count of ref as -1
				page_tmp->ref_ct=-1;
				//set occupied as false
				page_tmp->occup=PG_FREE;
				//link the next free to next struct
				page_tmp->next=page_tmp+1;
			}
			//last unused page
			else{
				page_tmp->ref_ct=-1;
				page_tmp->occup=PG_FREE;
				//no next page
				page_tmp->next=NULL;
			}
		}
		index++;
	}

	//make the free_pg_head as first unused page's linked list rep: 
	//address of free_pg_head 0X7FFF80212180
	free_pg_head=(page_t*)(physical_page_start+number);
	//free_pg_head = (unsigned long)free_pg_head + VIRT_ST;

	kprintf("free_pg_head %x \n",free_pg_head);
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



uint64_t get_ref_ct(uint64_t index){
	page_t *page_tmp;
	page_tmp=physical_page_start+index;
	return page_tmp->ref_ct;
}

void increase_ref_ct(uint64_t index){
	page_t *page_tmp;
	page_tmp=physical_page_start+index;
	page_tmp->ref_ct=(page_tmp->ref_ct)+1;	
}

void decrease_ref_ct(uint64_t index){
	page_t *page_tmp;
	page_tmp=physical_page_start+index;
	page_tmp->ref_ct=(page_tmp->ref_ct)-1;	
} 












