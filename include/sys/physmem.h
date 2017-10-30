#ifndef _PHYSMEM_H
#define _PHYSMEM_H

struct page{
  unsigned long occup;
  struct page* next;
  unsigned long pg_index;
  unsigned long ref_ct;
}__attribute__((packed));
typedef struct page page_t;

unsigned long allocate_page(unsigned int physfree_pg_start);
unsigned int get_free_pg(page_t* head);