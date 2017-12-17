#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#define BUFSIZE 100

int main(int argc, char* argv[], char* envp[]){    
    //char buf[BUFSIZE] = {0};
    //getcwd(buf, BUFSIZE);
    //printf("@@@@@@@@@@@@@@@@@@@@@@@2current cwd %s\n", buf);
    //printf("%s\n", argv[0]);
        //printf("%s\n", argv[1]); 
        //while(1); 
    if(!strcmp(argv[1],"0")){
    	listfiles(argv[0]);
    }
    else{
    	listfiles(argv[1]);
    }

	return 0;
}