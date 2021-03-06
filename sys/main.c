#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/io.h>
//#include <sys/pic.h>
#include <sys/idt.h>
#include <sys/timer.h>
#include <sys/irq.h>
#include <sys/pci.h>
#include <sys/physmem.h>
#include <sys/virtualmem.h>
#include <sys/process.h>
#include <sys/terminal.h>
#include <sys/syscall.h>
#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
extern task_struct* first;
//@todo: declare these structs will divide the availbe memory into 3 parts 
page_t* physical_page_start;
page_t* free_pg_head;

int x=0,y=0;
unsigned long memory_length=0;
unsigned long smap_base_max = 0;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  //kprintf("physbase!!! %d \n", (uint64_t)physbase);
  //kprintf("physfree %p \n", (uint64_t)physfree);
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("smap->length %x\n", smap->length);
      kprintf("smap->base %x\n", smap->base);
      kprintf("physfree %x\n", physfree);
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      if(smap->length>memory_length){
        memory_length=smap->length;	
        smap_base_max = smap->base;
      }
    }
  }

//page_index = 256 here
unsigned long page_index=smap_base_max>>12;
//kprintf("page_inde x%d \n",page_index);

//memory_length = 24284 pg
//memory_length=(memory_length>>12)-PG_DESC_SIZE;
//kprintf("memory length %d \n",memory_length);

//physical_page_start ,,m  & physfree =  0x20c000
physical_page_start= (page_t*)(0xffffffff80000000UL + physfree);
//kprintf("physical_page_start %x \n",physical_page_start);

//page_total_number = 24284 pg
unsigned long page_total_number=(memory_length>>12)-PG_DESC_SIZE;

//used_page = 524 pgs, where is number of pages before physfree
unsigned long used_page=(unsigned long)physfree>>12;
//kprintf("used_page %d \n",used_page);

//@todo:used page +257?

//used page = 780, we mark the next 256 pg after physfree as used. Because we are going to use the 256 pgs to store 4 layers tables
used_page=used_page+256;

//param (used_page: 780,page_index: 256,page_total_number: 24284)
//init_phy(used_page,page_index,page_total_number);
init_phy(used_page,0,page_total_number);


kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
kprintf("physbase %p\n", (uint64_t)physbase);
kprintf("physfree %p\n", (uint64_t)physfree);


kprintf("memory_length %x main", memory_length);
//physfree =  0x20c000
init_kernalmem(physfree);

init_virt_phy_mapping(memory_length);

init_tarfs();
//while(1);

set_msr();

terminal_init();

init_pid();


task_struct* idle = create_init_kthread();
uint64_t sr=0x0000FF0000000000;
uint64_t msk=0xFFFF007FFFFFFFFF;
uint64_t vir=0xFFFFFF8000001000;
int level=1;
int i = 0;

    for (i = level; i > 0; i--) {
        vir >>= 9;
        vir &= msk;
        vir |= sr;
        kprintf("%x  ",vir);
        // dprintf ("i is %d\n", i);
    }

    vir |= 0xFFFF000000000000;
    vir &= 0xFFFFFFFFFFFFFFF8;

kprintf("%x",vir);
/*
uint64_t a= 0xFFFFFFFF80000000;
uint64_t b=(((a) >> 39) & 0x1FF);
uint64_t c=(((a) >> 12) & 0x1FF);

kprintf("   %x, %x",b,c);*/





//  kprintf("a");

/*
task_struct* idle = create_kthread(func_a);
task_struct* t2 = create_kthread(print_kthread);
schedule();
*/

//schedule();
//task_struct* idle2 = create_kthread(print_kthread);

//kprintf("pid1 %d \n",idle->pid);
//kprintf("pid2 %d \n",idle2->pid);

  //checkAll();

/*int a=4779;
int b=15;
int *p=&a;
char s[]="abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde";
uint64_t p2=(uint64_t)p;
kprintf("%s",s);
kprintf("%d+%d\n",a,b);
kprintf("%x\n",a);
kprintf("%p\n",p);*/
while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
//kprintf("bbbbbbbbbbbbbbbbbbbbbbbb");
  
__asm__("sti");

init_gdt();
idt_clear();
idt_load();


idt_install();
irq_install();
//idt_install();


//isrs_install();
timer_phase();

//checkAll();
//kprintf("XXXXXXXXXXXXXXXXXXXXXXXXX");


//__asm__ __volatile__ ("int $32");
//__asm__ __volatile__ ("int $32");
//__asm__ __volatile__ ("int $32");

//kprintf("address:%d",p2);
//kprintf("address:%p",p);
/*kprintf("aaa\nc%d",a);
kprintf("aaa\nc%d",a);
kprintf("aaa\nc%d",a);
kprintf("aaac%d\n",a);
kprintf("123\r%d",b);*/
//kprintf("x%d",x);
//kprintf("y%d",y);





  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  /*for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;*/

//int a=4779;
//int *p=&a;
//char s[]="abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde";
//uint64_t p2=(uint64_t)p;
//kprintf("%s",s);
//kprintf("%x\n",a);
//kprintf("%c",c);
//kprintf("%p\n",p);

  while(1);
}
