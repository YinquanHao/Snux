#include <stdio.h>

#define TABLE_SIZE 512 //512 entries for each TABLE_SIZE
#define PAGE_SIZE 0x1000 //4KB for each PAGE_SIZE
#define KERNAL_MAX_PAGE 128 //the maximum number of pages for kernal

struct pml4t {
    unsigned long  PML4E[TABLE_SIZE];
};
typedef struct pml4t* pml4_t;

struct pdpt {
	unsigned long PDPTE[TABLE_SIZE];
};
typedef struct pdpt* pdpt_t;

struct pdt {
	unsigned long PDTE[TABLE_SIZE];
};
typedef struct pdt* pdt_t;

struct pt {
	unsigned long PTE[TABLE_SIZE];
};
typedef struct pt* pt_t;


// this kernalmap used to record how many pages used by kernal right now;
int kernalmap[KERNAL_MAX_PAGE];


void * get_physaddr(void* virtualaddr);
int is_align(size_t size, void * phyaddr);
void *kmalloc(size_t size, void *physfree);
void * init_pml4(void * phyaddr);
void * init_pdpt(void * phyaddr, pml4_t PML4, unsigned long pml4_index);
void * init_pdt(void * phyaddr, pdpt_t PDPT, unsigned long page_dir_pt_index);
void * init_pt(void * phyaddr,pdt_t  PDT, unsigned long page_dir_index);
void * init_mm(void* phyaddr, pml4_t PML4, pdpt_t PDPT, pdt_t PDT, pt_t PT);
unsigned long  get_kmalloc_base(unsigned long physfree, unsigned long smap_base);



void * init_mm(void* phyaddr, pml4_t PML4, pdpt_t PDPT, pdt_t PDT, pt_t PT){
	PML4 = (pml4_t)kmalloc(PAGE_SIZE,phyaddr);
	printf("the address of PML4 %lu \n", (unsigned long)PML4);
	printf("PML4->PMLE[0] %lu \n",PML4->PML4E[0]);

	void* PDPT_addr = init_pdpt(phyaddr,PML4,0);
	printf("enter this");
	printf("the address of PDPT %lu \n", (unsigned long)PDPT_addr);

	PDT = init_pdt(phyaddr,PDPT,0);
	printf("the address of PDT %lu \n", (unsigned long)PDT);

	PT = init_pt(phyaddr,PDT,0);
	printf("the address of PT %lu\n", (unsigned long)PDT);


}

/*initialize pml4 table*/
void * init_pml4(void * phyaddr){
	return (void*)kmalloc(PAGE_SIZE, phyaddr);
}

/*initialize pdpt table*/
void * init_pdpt(void * phyaddr, pml4_t PML4, unsigned long pml4_index){
	printf("here0 \n" );
	void* base = kmalloc(PAGE_SIZE, phyaddr);
	printf("here1  %lu \n", (unsigned long)base );
	PML4->PML4E[pml4_index] = 0;
	printf("here2 \n" );
	return base;
}

/*initialize pdt table*/
void * init_pdt(void * phyaddr, pdpt_t PDPT, unsigned long page_dir_pt_index){
	void* base  =  kmalloc(PAGE_SIZE,phyaddr);
	PDPT->PDPTE[page_dir_pt_index] = (unsigned long)base;
	return base;
}

/*initialize pt table*/
void * init_pt(void * phyaddr, pdt_t PDT, unsigned long page_dir_index){
	void * base = kmalloc(PAGE_SIZE,phyaddr);
	PDT->PDTE[page_dir_index] = (unsigned long)base;
	return base;
}



/*check if a physical address id aligment of size*/
int is_align(size_t size, void * phyaddr){
	if(*(unsigned long*)phyaddr % size==0){
		return 1;
	}else{
		return 0;
	}
}

//kmalloc: alloc a continuous memory of size
void *kmalloc(size_t size, void *physfree){
	printf("physfree %lu\n", *(unsigned long*)physfree );
	for(int i=0;i<KERNAL_MAX_PAGE;i++){
		printf("loop %d \n", i);
		printf("kernalmap[i] %d \n", kernalmap[i]);
		if(kernalmap[i]==1){
			continue;
		}else{
			kernalmap[i]=1;
			printf("loop1 %d \n", i);
			unsigned long base_addr = *(unsigned long*)physfree<<12 + i * PAGE_SIZE;
			printf("base_addr %lu \n", base_addr);
			for(int j=0;i<PAGE_SIZE;j++){
				int* ptr = (int*)(base_addr+j);
				*ptr = 0; 
			}
			return (void*)(base_addr);
		}
	}
}

/*map a virtual address to a physical address*/
void * map_page(void * phyaddr, void * virtualaddr){
	printf("virtualaddr %lu\n", *(unsigned long*)virtualaddr);
	unsigned long pml4_index = *(unsigned long*)virtualaddr >> 39 & 0x1FF;
	printf("pml4_index %lu\n", pml4_index);
	unsigned long page_dir_pt_index  =  *(unsigned long*)virtualaddr >> 30 & 0x1FF;
	printf("page_dir_pt_index %lu\n", page_dir_pt_index);
	unsigned long page_dir_index  = *(unsigned long*)virtualaddr >> 21 & 0x1FF;
	printf("page_dir_index %lu\n", page_dir_index);
	unsigned long page_tb_index =  *(unsigned long*)virtualaddr >> 12 & 0x1FF;
	printf("page_tb_index %lu\n", page_tb_index);




}



void* get_physaddr(void * virtualaddr){
	printf("virtualaddr %lu\n", *(unsigned long*)virtualaddr);
	unsigned long pml4_index = *(unsigned long*)virtualaddr >> 39 & 0x1FF;
	printf("pml4_index %lu\n", pml4_index);
	unsigned long page_dir_pt_index  =  *(unsigned long*)virtualaddr >> 30 & 0x1FF;
	printf("page_dir_pt_index %lu\n", page_dir_pt_index);
	unsigned long page_dir_index  = *(unsigned long*)virtualaddr >> 21 & 0x1FF;
	printf("page_dir_index %lu\n", page_dir_index);
	unsigned long page_tb_index =  *(unsigned long*)virtualaddr >> 12 & 0x1FF;
	printf("page_tb_index %lu\n", page_tb_index);
}


unsigned long  get_kmalloc_base(unsigned long physfree, unsigned long smap_base) {
	physfree = (physfree>>12)+((physfree-smap_base)>>12);
	return physfree<<12;
}


int main(){
	/*
	pdpt_t PDPT;
	pdt_t PDT;
	pt_t PT;


	unsigned long virtualaddr = 0x80801fffa8;
	void* virtualaddr_ptr = (unsigned long*)&virtualaddr;
	get_physaddr(virtualaddr_ptr);
	printf("the size_of plm4 %lu \n", sizeof(pml4_t));


	unsigned long phyaddr = 0x4000;
	void* phyaddr_ptr = (unsigned long*)&phyaddr;
	int align = is_align(PAGE_SIZE,phyaddr_ptr);
	printf("check is_align %d \n", align);


	pml4_t PML4 = kmalloc(PAGE_SIZE ,phyaddr_ptr);
	printf("PML4->PMLE[0] %lu \n",PML4->PML4E[0]);




	void*  add = kmalloc(PAGE_SIZE,phyaddr_ptr);
	printf("the address of mallock %lu \n", (unsigned long)add);
	void*  add1 = kmalloc(PAGE_SIZE,phyaddr_ptr);
	printf("the address of mallock %lu \n", (unsigned long)add1);
	*/
	printf("print %lx",get_kmalloc_base(0X20B000,0x100000));

	//init_mm(phyaddr_ptr,PML4,PDPT,PDT,PT);


}

