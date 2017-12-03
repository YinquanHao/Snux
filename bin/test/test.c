#include <stdlib.h>
#include <stdio.h>
int main(){
	int a =0;
	char b='t';
	char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	write(1, &b, 1);
	a=a+1;
	write(1,&buf,5);
	a=a+2;
	printf("%d",a);
	while(1);
	return a;
}