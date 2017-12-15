#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
int main(int argc, char* argv[], char* envp[]){
	if(argv[0]==NULL){
			printf("not");
	}
	printf("%s\n", argv[0]);
	cat(argv[0]);
	return 0;
}
