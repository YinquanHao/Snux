#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/pic.h>


/*void pic_remap(){
 	outb(PIC1, 0x11);
    outb(PIC2, 0x11);
    outb(PIC1_DATA, PIC1);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}*/


void irq_install(){
	pic_remap();
	idt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);
}
