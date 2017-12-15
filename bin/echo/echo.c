#include <stdio.h>
#include <stdlib.h>
//print the argv
int main(int argc, char* argv[], char* envp[]) {
	int len=toInt(argv[0]);
	//printf("%d\n",len );
	for(int i=0;i<len-1;i++){
        printf(" %s ",argv[i+1]);
    }
    return 0;
}