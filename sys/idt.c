#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/kprintf.h>

struct idt_entry idt[256];

//idt_entry_t idt[256] = {};
static struct idt_ptr idtr={
    sizeof(idt),
    (uint64_t)idt,
};



void _x86_64_asm_lidt(struct idt_ptr *idtr);
//void idt_load();


//check
void idt_load(){

	_x86_64_asm_lidt(&idtr);
	//kprintf("loadddddddddddddddddddddddd");
}


//check
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags){
    idt[num].offset_1 = (base & 0xFFFF);
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
    idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    //TODO: do we need to make it OR\0x60
    idt[num].type_attr = flags;
    idt[num].zero=0;
}



void idt_clear(){
	for(int i=0;i<256;i++){
		idt[i].offset_1 = 0;
    	idt[i].offset_2 = 0;
    	idt[i].offset_3 = 0;
    	idt[i].selector = 0;
    	idt[i].ist = 0;
    	idt[i].type_attr = 0;
    	idt[i].zero=0;
	}
    pic_remap();
//pci remap???
}

void idt_install(){
//	idt_clear();
    
idt_set_gate(0,(uint64_t)isr0,0x08,0x8E);//8E to EE?
idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);

//kprintf("idt_install??????????????????????????????????????????????????????????????????????????????");
//idt_set_gate(2, (uint64_t)isr1, 0x08, 0x8E);
/*idt_set_gate(3, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(4, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(5, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(6, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(7, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(8, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(9, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(10, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(11, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(12, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(13, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(14, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(15, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(16, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(17, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(18, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(19, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(20, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(21, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(22, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(23, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(24, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(25, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(26, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(27, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(28, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(29, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(30, (uint64_t)isr1, 0x08, 0x8E);
idt_set_gate(31, (uint64_t)isr1, 0x08, 0x8E);*/
//idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
   // idt_set_gate(32, (uint64_t)isr32, 0x08, 0x8E);
//	idt_load();
	//_x86_64_asm_lidt(&idtr);
	//idt_set_gate()

}

