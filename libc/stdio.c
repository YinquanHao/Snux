#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>
int printf(const char *format, ...){
	int res=0;
	long hex;
	int  i;
	char *s;
	char c;
	int len=0;
	va_list arg; 
	va_start(arg, format); 
	while(*format){
		if(*format=='%'){
			format++;
			switch(*format){
				case 'd':
					i=va_arg(arg,int);
					if(i<0){
						printchar('-');
						i=-i;
					}
					len=intgerLength(i);
					convertInt(i,len);
					break;
				case 's':
					s=va_arg(arg,char*);
					convertStr(s);
					break;
				case 'c':
					c=va_arg(arg,int);
					printchar(c);
					break;
				case 'x':
					hex=va_arg(arg,long);
					convertHex(hex);
					break;
				default:
					res=-1;
					break;
			}

			format++;
		}
		else{
			printchar(*format);
			format++;
		}
		if(res==-1){break;}
		}
	va_end(arg);
return res; 

}

void convertInt(int num,int length){
//int length=intgerLength(num);
	int num_temp=num;
	char buf[10];
	for(int i=0;i<10;i++){buf[i]=0;}
	buf[9]='\0';	
	int p=8;
	while(num_temp/10){
		buf[p]=num_temp%10;
		num_temp=num_temp/10;	
		p--;	
	}
	buf[p]=num_temp;
	int m=9-length;
	while(m<9){
		printchar(buf[m]+'0');
		buf[m]=0;
		m++;
	}
//convertStr(buf);
//return;
}

/*void convertInt(int num){

}*/


void convertHex(long num){
	char buf[32]={0};
	long num_temp=num;
	long t=0;
	int cnt=0;
	char *s;
	char *s1=buf;
	s=buf+32-1;
	*s='\0';
	while (num_temp) {
		t = num_temp % 16;
		if( t >= 10 )
		t += 'A' - '0' - 10;
		*--s = t + '0';
		num_temp /= 16;
		cnt++;
	}
//while(num_temp/16)
	convertStr("0x");
	//convertStr(s);
	for(int i=0;i<cnt;i++){
		printchar(*s);		
		*s=0;
		s++;
	}
/*	for(int i=0;i<31;i++){
		*s1=0;
		s1++;
	}*/
}



void convertStr(char* str){
	while(*str!='\0'){
		printchar(*str);
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

unsigned long toInt(const char* str)
{
    unsigned long mult = 1;
    unsigned long re = 0;
    int len = strlen(str);
    for(int i = len -1 ; i >= 0 ; i--)
    {
        re = re + ((int)str[i] -48)*mult;
        mult = mult*10;
    }
    return re;
}

char *gets(char *s){
	syscall_1(SYS_gets,s);
	return s;
}

void *memset(void *s, int ch , unsigned long n) {
    char* tmp = s;
    while(n > 0) {
        *tmp = ch;
        tmp++;
        n--;
    }
    return s;
}

int puts(const char *s){
	printf("%s", s);
}