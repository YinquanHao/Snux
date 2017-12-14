
#include <stdio.h>
#include <stdlib.h>
//print the argv
int main(int argc, char* argv[], char* envp[]) {
	for(int i=1;i<argc;i++){
        printf("%s ",argv[i] );
    }
    return 0;
}