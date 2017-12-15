#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]){
    //printf("hello!!!!!!!!!!!!!!!!!!!!1\n" );
    //while(1);
	
	//char *str1="aaaaaaaabcd";
	//puts(str1);

/*	char str2[128]={0};
	gets(str2);
	printf("2:\n%s\n",str2);*/

	char str3[60]={0};
	read(0,str3,60);
	printf("%s\n",str3);
    return 0;
}