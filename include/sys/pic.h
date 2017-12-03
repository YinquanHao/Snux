#ifndef _PIC_H
#define _PIC_H
#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#include <sys/io.h>


static inline void pic_remap(){
    __asm__ __volatile__("cli");
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
    __asm__ __volatile__("sti");



}



static inline void PIC_sendEOI(unsigned char irq){
	if(irq >= 8){
	outb(PIC2_COMMAND,PIC1);
 	outb(PIC1_COMMAND,PIC1);
	}
	else{
	outb(PIC1,PIC1);
	}
}


#endif
