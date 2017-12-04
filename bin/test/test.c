#include <stdlib.h>
#include <stdio.h>
int main(){
	/*
	int a =0;
	char b='t';
	char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	write(1, &b, 1);
	a=a+1;
	write(1,&buf,5);
	a=a+2;
	printf("%d",a);
	char* test = 0x00000FFF80001000;
	printf("%s",test);
	*/
	char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	write(1, &buf, 1);
	char* str = (char*)malloc(25);
	while(1);
	str[0] = 'c';
	printf("%c\n", str);
	while(1);
	//return a;
}