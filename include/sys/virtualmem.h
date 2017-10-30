#ifndef _VIRTUALMEM_H
#define _VIRTUALMEM_H

struct page{
  unsigned long occup;
  struct page* next;
  unsigned long pg_index;
  unsigned long ref_ct;
}__attribute__((packed));
typedef struct page page_t;


