#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#define BUFSIZE 100

int main(/*int argc,char* argv[]*/){    
    char buf[BUFSIZE] = {0};
    getcwd(buf, BUFSIZE);
    listfiles(buf);
	return 0;
}