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

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

int x=0,y=0; 

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
     kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
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
//pic_remap();
idt_load();

idt_install();
irq_install();
//isrs_install();
//timer_phase();

checkAll();
//kprintf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");




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
