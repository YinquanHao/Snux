#include <sys/terminal.h>
#include <sys/virtualmem.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDR 0xB8000+VIRT_ST
extern int x,y;//x is the column, y is the row num
int tx,ty;
terminal* init_t;//init terminal
char tbuffer[1024]={0};
int cursor=0;
int scan_flag=0;
void clear_terminal(){
	kprintf("enter");
	uint64_t start=(unsigned short *)(VGA_ADDR);
	x=0;
	y=0;
	//while(1);
	memset((void *)start,0,4096);
}

void terminal_init(){
	clear_terminal();
	//terminal* init_terminal;
	//while(1);
	//kprintf("aa");	
	tx=0;
	ty=20;
	//init_t->tbuffer[1024]={0};
}

uint64_t terminal_write(int fd, char *buf, int count){
	uint64_t len=strlen(buf);
	kprintf("%s",buf);
	return len;
}

//stdin
uint64_t terminal_read(char *buf, int count) {
    __asm__ __volatile__ ("sti");
    scan_flag = 1; //set local echo flag
    memcpy((void*) buf, (void*) tbuffer, count);
        //while(1);
    uint64_t res = count > cursor ? cursor : count; 
    //__asm__ __volatile__ ("cli");
    return res;
}

void terminal_input(char input_state,char input_char){
	if(input_state=='^'&&input_char=='a'){
		kprintf("%s",tbuffer);
		return;
	}
	if(input_char=='\b'){}
	tbuffer[cursor]=input_char;
	cursor++;

	char buf[2]={0};
	buf[0]=input_char;
	unsigned short * textptr = (unsigned short *)(VGA_ADDR)+tx+ty*VGA_WIDTH;
	//kprintf("%d",tx);
	//while(1);
	unsigned char *c = (uint8_t *)buf;
	while (*c!='\0') {
		if(*c=='\n'){
			//kprintf("%c",buf[0]);			
			textptr=textptr-tx+VGA_WIDTH;
			//t_scroll();
			c++;
			ty++;
			tx=0;
			//excute
			//cursor=0;
			//memset(&tbuffer[0],0,100);
			//break;			
		}
/*		else if(*c=='1'){
			kprintf("xxxxx");
			*textptr++ = *c|0xF1<<8;
			c++;
			tx++;
			break;
			//break;
		}*/
/*		else if(*c=='2'){
			char *a="111";
			terminal_read(a,10);
			kprintf("%s",a);
			break;
		}*/
		else if(*c=='\b'){//backspace
			backspace(textptr);
			break;
		}
		else if(*c=='\r'){
			textptr=textptr-tx;
			c++;			
			tx=0;
		}else{
			*textptr++ = *c|0xF1<<8;
			c++;
			tx++;

		}
		if(tx==80){
			tx=0;
			ty++;
		}
		if(ty>22){
			//kprintf("scroll");
			t_scroll();
			ty--;
		}
	}

}

void t_scroll(){
	uint64_t loc=(unsigned short *)(VGA_ADDR)+19*VGA_WIDTH;
	memset((void *)loc,0,160);
	int line_num=ty;
	for(int i=21;i<line_num;i++){
		move_line(i,i+1);
	}
	loc=(unsigned short *)(VGA_ADDR)+(ty-1)*VGA_WIDTH;
	memset((void *)loc,0,160);
}

void backspace(unsigned short  *text){
			if(tx==0&&ty==20){
				return;
			}
			tbuffer[cursor]=0;
			cursor--;
			tbuffer[cursor]=0;
			cursor--;
			*text--;
			*text=0;
			//cursor--;
			tx--;
}