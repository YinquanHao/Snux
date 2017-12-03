#include <sys/keyboard.h>
#include <sys/kprintf.h>
#include <sys/io.h>

//int shift,control;

	volatile int shiftPressed = 0;
	volatile int ctlPressed = 0;
/*void keyboard_handler(){
	char c;
	c=inb(0x60);
	if((int)c==0x2A){
		kprintf("shift!");
		shift=1;			
	}
	else if(){
	}
	else{
	kprintf("%c",kb[(int)c]);
	outb(0x20, 0x20);
}*/

void handle_keyboard(){
	//kprintf("enterkb");
	unsigned char input = inb(PS2_RW);

	char state=' ';
	char key=' ';
	//kprintf("input:%x",input);
	if(input == 0x36 || input == 0x2A){
		shiftPressed = 1;
		//kprintf("shift!");
	}else if(input == 0xAA || input == 0xB6){
		shiftPressed = 0;
	}else if(input == 0x1D){
		ctlPressed = 1;
		//kprintf("ctrl!");
	}else if(input == 0x9D){
		ctlPressed = 0;
	}else if(input>=F1 && input<=F10){
		kprintf("enter F");
		state = 'F';
		key = (int)input-F1+1;
	}
/*else if(ctlPressed==1&&input==0x32){
	state = '^';key = scan_code[input];kprintkb(state,key);PIC_sendEOI(33);
}*/
else{
		//kprintf("state:%c",state);
		if(ctlPressed==1) state = '^';
		if(shiftPressed==0){
			key = scan_code[input];

		}else if(shiftPressed ==1){
			key = scan_code_sh[input];
		}
	PIC_sendEOI(33);
	if(input<0x81){

			terminal_input(state,key);
		
		}
	}
	PIC_sendEOI(33);

}
