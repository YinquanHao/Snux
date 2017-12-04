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
	//char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	//write(1, &buf, 1);
	char* str;
	str = (char*) malloc(25);
	strcpy(str,"hello, world");
	//while(1);
	//str = "hello, world";
	//printf("%s\n", str);
	//free((void*)str);
	printf("123123213123123 %s \n",str);
	//free((void*)str);
	char* str1;
	str1 = (char*) malloc(25);
	strcpy(str1,"dlrow ,olleh");
	printf("this:%s \n",str);
	printf("this:%s \n",str1);
	char* str2;
	str2 = (char*) malloc(25);
	strcpy(str2,"qwueuiqwieu");
	printf("this:%s \n",str2);

	while(1);
	return 1;
}