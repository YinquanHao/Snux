#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#define BUFSIZE 100

int main(int argc, char* argv[], char* envp[]){    
    char buf[BUFSIZE] = {0};
    getcwd(buf, BUFSIZE);
    printf("@@@@@@@@@@@@@@@@@@@@@@@2current cwd %s\n", buf);
    listfiles(buf);
	return 0;
}