//#include <sys/defs.h>
#include <sys/io.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/process.h>

int ticks=0;
int second=0;

void timer_phase(){
	int hz=1000;	
	int divisor=1193182/hz;
	//kprintf("divisor:%d",divisor);
	outb(0x43, 0x36);             /* Set command register 0x36 (mode 3) */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

void timer_install(){
	ticks++;
	//kprintf("total:%d",ticks);
	if(ticks%1000==0){
		//kprintf("1s ");
		//kprintf("total:%d",ticks);
		clear_sleep();
		second++;
		kprintt("%75d",second);
	}
	/*if(second==60){
		kprintf(" 1min");
}*/
    //outb(0x20, 0x20);
    PIC_sendEOI(32);
	

}
