#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int printf(const char *format, ...){
	int res=0;
	int  i;
	char *s;
	char c;
	va_list arg; 
	va_start(arg, format); 
	while(*format){
		if(*format=='%'){
			format++;
			switch(*format){
				case 'd':
					i=va_arg(arg,int);
					if(i<0){
						putChar('-');
						i=-i;
					}
					convertInt(i);
					break;
				case 's':
					s=va_arg(arg,char*);
					convertStr(s);
					break;
				case 'c':
					c=va_arg(arg,int);
					putChar(c);
					break;
				default:
					//putChar('e');
					res=-1;
					break;
			}

			format++;
		}
		else{
			putChar(*format);
			format++;
		}
		if(res==-1){break;}
	}
	va_end(arg);
	return res; 

}

void convertInt(int num){
	int length=intgerLength(num);
	char buf[10];
	for(int i=0;i<10;i++){buf[i]=0;}	
	int p=9;
	while(num/10){
	buf[p]=num%10;
	num=num/10;	
	p--;		
	}
	buf[p]=num;
	p=10-length;
	while(p<10){
	putChar(buf[p]+'0');
	p++;
	}

}

void convertStr(char* str){
	while(*str!='\0'){
		putChar(*str);
		str++;
	}

}


int intgerLength(int i){
 	if (i <         10) return 1;
 	if (i <        100) return 2;
 	if (i <       1000) return 3;
 	if (i <      10000) return 4;
 	if (i <     100000) return 5;
 	if (i <    1000000) return 6;      
 	if (i <   10000000) return 7;
 	if (i <  100000000) return 8;
 	if (i < 1000000000) return 9;
 	return 10;
}


