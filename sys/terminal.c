#include <sys/terminal.h>
#include <sys/virtualmem.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDR 0xB8000+VIRT_ST
extern int x,y;//x is the column, y is the row num
int tx,ty;
int enter_press=0;
terminal* init_t;//init terminal
char tbuffer[8][128]={0};
int tbufferNum=0;
int cursor=0;
int pre_cur=0;
char* t_current=tbuffer;
int scan_flag=0;

void clear_terminal(){
	//kprintf("enter");
	uint64_t start=(unsigned short *)(VGA_ADDR);
	x=0;
	y=0;
	//while(1);
	memset((void *)start,0,4096);
}

void terminal_init(){
	//clear_terminal();
	//terminal* init_terminal;
	//while(1);
	//kprintf("aa");	
	tx=0;
	ty=20;
	//init_t->tbuffer[1024]={0};
}

uint64_t terminal_write(int fd, char *buf, int count){
	uint64_t len=strlen(buf);
	//kprintf("%x",buf);
	kprintf("%s",buf);
	return len;
}

//stdin
/*uint64_t terminal_read(char *buf, int count) {
	uint64_t res;    
    if(scan_flag==1&&enter_press==1){
    	 //kprintf("%x %x",t_current,tbuffer);
    	 memcpy((void*) buf, (void*)tbuffer, count);
    	 res = count > cursor ? cursor : count;
    	 cursor=0;
    	 enter_press=0;
    	 scan_flag=0;
    	 clear_tbuffer();
    }
    else if(scan_flag==0&&enter_press==1){//only 
    	enter_press=0;
    	res=0;
    	tbuffer[cursor]=0;
		cursor--;
    }
    else{
    	res=0;
    }
    return res;
}*/
uint64_t terminal_read(char *buf, int count) {
	uint64_t res;    
    if(scan_flag==1&&enter_press==1){
    	 //kprintf("%x %x",t_current,tbuffer);
    	 memcpy((void*) buf, (void*)tbuffer[tbufferNum], count);
    	 res = count > cursor ? cursor : count;
       	 //cursor+=res;
    	 enter_press=0;
    	 scan_flag=0;
    	 cursor=0;
    	 tbufferNum++;
    	 if(tbufferNum==8){
    	 	clear_tbuffer();
    	 	tbufferNum=0;
    	 }
    	 //clear_tbuffer();
    }
    else if(scan_flag==0&&enter_press==1){//only 
    	enter_press=0;
    	res=0;
    	tbuffer[tbufferNum][cursor]=0;
		cursor--;
    }
    else{
    	res=0;
    }
    return res;
}



uint64_t standard_input(char *buf,int count){
	uint64_t res;
	memcpy((void*) buf, (void*)tbuffer, count);
	res = count > cursor ? cursor : count;
	return res;
}

uint64_t getsline(char* buf){
	memcpy((void*) buf, (void*)tbuffer[tbufferNum], 128);
}


void terminal_input(char input_state,char input_char){
	if(input_state=='^'&&input_char=='a'){
		kprintf("%s",tbuffer);
		return;
	}
	//if(input_char=='\b'){}
	/*tbuffer[cursor]=input_char;
	cursor++;*/
	tbuffer[tbufferNum][cursor]=input_char;
	cursor++;
	char buf[2]={0};
	buf[0]=input_char;
	unsigned short * textptr = (unsigned short *)(VGA_ADDR)+tx+ty*VGA_WIDTH;
	//kprintf("%d",tx);
	//while(1);
	unsigned char *c = (uint8_t *)buf;
	while (*c!='\0') {
		if(*c=='\n'){
			textptr=textptr-tx+VGA_WIDTH;
			c++;
			ty++;
			tx=0;
			enter_press=1;


/*s*/
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
    	 	if(enter_press==1){
    	 		enter_press=0;
    	 	}
    	 	scan_flag=1;
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
			if(tx==0/*&&ty==20*/){
				tbuffer[tbufferNum][cursor]=0;
				cursor--;
				return;
			}
			tbuffer[tbufferNum][cursor]=0;
			cursor--;
			tbuffer[tbufferNum][cursor]=0;
			cursor--;
			*text--;
			*text=0;
			//cursor--;
			tx--;
}

void clear_tbuffer(){
	for(int j=0;j<8;j++){
		for(int i=0;i<128;i++){
			tbuffer[j][i]=0;
		}
	}	
}