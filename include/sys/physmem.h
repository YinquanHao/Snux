#ifndef _PHYSMEM_H
#define _PHYSMEM_H

#define NULL ((void*)0)
#define PG_OCCU 1
#define PG_FREE 0
struct page{
  unsigned long occup;
  struct page* next;
  unsigned long pg_index;
  unsigned long ref_ct;
}__attribute__((packed));
typedef struct page page_t;

unsigned long allocate_page(unsigned int physfree_pg_start);
unsigned int get_free_pg(page_t* head);
void init_phy(unsigned long  number,unsigned long  index,unsigned long  page_max_number);
int free_page(unsigned long index);


#endif
