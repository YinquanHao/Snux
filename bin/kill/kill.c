#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]){
	//printf("%d\n",argv[0]);
	int pid= toInt(argv[0]);
	if(argv[0]==NULL){
		return -1;
	}			
	int res=kill(pid,pid);
	return 0;
}
