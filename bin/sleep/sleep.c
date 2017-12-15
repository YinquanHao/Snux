#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]) {
   int stime =toInt(argv[0]);
   //printf("%s  %s  %s\n",argv[0],argv[1],argv[2]);
   printf("stime:%d\n", stime);
   //stime=2;
   if(stime<=0){
   	printf("time need >0");
   }
   else{
   	sleep(stime);
   }
   return 0;
}