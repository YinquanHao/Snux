#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>
#define UINT32_MAX 4294967295
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
extern int x,y;

void print_int(unsigned int input, unsigned int format_length, char* display_buf, int* ptr);
void printWrapper(char* display_buf, const char* format, va_list args);
unsigned int strlen_a(const char* input);
void print_char(int input, char* display_buf, int* ptr);
void print_p(unsigned int input, char* display_buf, int* ptr);
void print_hex(unsigned int input, unsigned int format_length, char* display_buf, int* ptr);
void print_s(char* input, char* display_buf, int* ptr);
void kprintt(const char *fmt,...);

void print_int(unsigned int input, unsigned int format_length, char* display_buf, int* ptr){
	/*the length of the input value*/
	unsigned int trueLength = 1;
	/*the max value which has the same length as the input value*/
	unsigned int trueMax = 9;
	/*iterat thought the input value,
	 keep generate the trueMax while 
	 trueMax is less than the input*/
	while(trueMax < UINT32_MAX && trueMax<input){
		trueMax = trueMax*10+9;
		trueLength+=1;
	}
	if(format_length<=trueLength){
		format_length = trueLength;
	}
	/*for the required format: If the 
	required format is longer than 
	the input number, then we need to 
	add the leading zeros into the buffer*/
	int format_length_cp = format_length;
	while(format_length_cp>trueLength){
		display_buf[*ptr] = '0';
		(*ptr)++;
		format_length_cp--;
	}
	/*atoi and store the res into buff*/
	int trueLength_cp = trueLength;
	while(trueLength_cp>0){
		unsigned int val = input%10;
		input /=10;
		display_buf[*ptr+trueLength_cp-1] = val + '0';
		//fprintf(stderr, "index %d\n", *ptr+trueLength-1);
		//fprintf(stderr, "value %d\n", val);
		trueLength_cp--;
	}
	(*ptr)=(*ptr)+trueLength;
}

void printWrapper(char* display_buf, const char* format, va_list args){
	//fprintf(stderr, "enter printWrapper\n");
	int buf_cursor = 0;
	for(int i=0;i<strlen_a(format);i++){
		if(format[i]!='%'){
			display_buf[buf_cursor++] = format[i];
			continue;
		}else{
			i++;
			unsigned int format_length = 0;
			while(format[i]-'0'>=0 && format[i]-'0'<=9){
				format_length = format_length*10+(format[i]-'0');
				i++;
			}
			if(format[i]=='d'){
				print_int(va_arg(args,unsigned long),format_length,display_buf,&buf_cursor);
			}else if(format[i]=='p'){
				print_p(va_arg(args, int ),display_buf,&buf_cursor);
			}else if(format[i]=='x'){
				print_hex(va_arg(args,unsigned long),format_length,display_buf,&buf_cursor);
			}else if(format[i]=='c'){
				print_char(va_arg(args,int),display_buf,&buf_cursor);
			}else if(format[i]=='s'){
				print_s(va_arg(args,char*),display_buf,&buf_cursor); 
			}
		}
	}
	display_buf[buf_cursor] = '\0';
	//return buf_cursor;
}


void kprintf(const char *fmt, ...){
	char buf[1024]={-1};
	va_list args;
	va_start(args, fmt);
	printWrapper(buf,fmt,args);
	va_end(args);
	unsigned short * textptr = (unsigned short *)0xB8000+x+y*VGA_WIDTH;
	unsigned char *c = (uint8_t *)buf;
	while (*c!='\0') {
//*textptr=0xF1;
//stextptr++;
		if(*c=='\n'){
			//*textptr = *c;				
			textptr=textptr-x+VGA_WIDTH;
			c++;
			y++;
			x=0;
		}
		else if(*c=='\r'){
			textptr=textptr-x;
			c++;			
			x=0;
		}else{		
			*textptr++ = *c|0xF1<<8;
			c++;
			x++;
		}
		if(x==80){
			x=0;
			y++;
		}
		//TODO(@yinquanhao) add scroll functionality.
	}
}

unsigned int strlen_a(const char* input){
	unsigned int ct = 0;
	while(input[ct++]!='\0');
	return ct-1;
}

void print_hex(unsigned int input, unsigned int format_length, char* display_buf, int* ptr){
	//fprintf(stderr, "enter print hex\n");
	/*the length of the input value*/
	unsigned int trueLength = 1;
	/*the max value which has the same length as the input value*/
	unsigned int trueMax = 15;
	/*hashtable for decimal to hex*/
	char alphabet[]= "0123456789ABCDEF";
	/*iterat thought the input value,
	 keep generate the trueMax while 
	 trueMax is less than the input*/
	while(trueMax < UINT32_MAX && trueMax<input){
		trueMax = trueMax*16+15;
		trueLength+=1;
	}
	/*make the minimal length as trueLength*/
	if(format_length<=trueLength){
		format_length = trueLength;
	}
	//fprintf(stderr, "index: %d\n",*ptr);
	/*hex mark*/
	display_buf[(*ptr)++] = '0';
	display_buf[(*ptr)++] = 'x';
	/*for the required format: If the 
	required format is longer than 
	the input number, then we need to 
	add the leading zeros into the buffer*/
	int format_length_cp = format_length;
	while(format_length_cp>trueLength){
		display_buf[*ptr] = '0';
		(*ptr)++;
		format_length_cp--;
	}
	/*atoh and store the res into buff*/
	int trueLength_cp = trueLength;
	while(trueLength_cp>0){
		unsigned int val = input%16;
		input /=16;
		display_buf[*ptr+trueLength_cp-1] = alphabet[val];
		trueLength_cp--;
	}
	(*ptr)=(*ptr)+trueLength;
}
void print_char(int input, char* display_buf,int* ptr){
	display_buf[*ptr] = input;
	(*ptr)++;
}

void print_s(char* input, char* display_buf, int* ptr){
	//fprintf(stderr, " print_s input :%s\n",input );
	int size = strlen_a(input);
	for(int i=0;i<size;i++){
		display_buf[(*ptr)+i] = input[i];
	}
	(*ptr)=(*ptr)+size;
}


void print_p(unsigned int input, char* display_buf,int* ptr){
	//fprintf(stderr, "intput : %d\n",input);
	/*intitialize the hashtable for hex*/
	char alphabet[]= "0123456789ABCDEF";
	/*the total length to print should be 14 chars long*/
	int len=14;
	/*the out put should start with 0x7....*/
	display_buf[(*ptr)] = '0';
	display_buf[(*ptr)+1] = 'X';
	display_buf[(*ptr)+2] = '7';
	/*fill all remaining char with F*/
	int i=3;
	while(i<14){
		display_buf[(*ptr)+i] = 'F';
		i++;
	}
	/*atoh*/
	while(input!=0){
		unsigned int val = input%16;
		input /=16;
		display_buf[*ptr+len-1] = alphabet[val];
		len--;
	}
	/*move pointer*/
	(*ptr)=(*ptr)+14;
}

void kprintt(const char *fmt,...){
	char buf[1024]={-1};
	va_list args;
	va_start(args, fmt);
	printWrapper(buf,fmt,args);
	va_end(args);

	unsigned short * textptr = (unsigned short *)0xB8000 +24*80;
	unsigned char *c = (uint8_t *)buf;
	while(*c=='0'){
		*textptr++ = ' ';
		c++;
	}
	while (*c!='\0') {
		*textptr++ = *c|0xF1<<8;
		c++;
	}

}

void kprintkb(char state,char key){
	//char buf[1024]={-1};
	//va_list args;
	//va_start(args, fmt);
	//printWrapper(buf,fmt,args);
	//va_end(args);

	unsigned short * textptr = (unsigned short *)0xB8000+25*80;
	*(textptr-1)=key|0xF1<<8;
	*(textptr-2)=state|0xF1<<8;


}





