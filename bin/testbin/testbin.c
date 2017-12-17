#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]){
    int status=0;
    /*dirent.h test*/
	DIR* dir1=opendir("/rootfs/bin");
	if(dir1==NULL){
        printf("open dir fail");
        return -1;
    }
    dirent *dir;
    dir = readdir(dir1);
    printf("Open dir successful: %s \n",dir->d_name);
    int closed=closedir(dir1);
    printf("Close dir successful: %d \n", closed);
 
	/*stdio.h test*/
	printf("Printf Test %s %d \n", "printftest",1);
	printf("puts test: ");
	puts("putstest \n");

	printf("Malloc Test & gets test: ");
	char *str1=malloc(128);
	gets(str1);// should be test, which is the last line input
	printf(" gets return %s \n",str1);

	printf("putchar Test: ");
	putchar(91);
	printf("\n");		
	
	/*unistd.h test*/
	/*open(),read(),write(),close(),unlink()*/
	printf("open file decriptor test \n");
	int fd=open("rootfs/tobedel.txt",2);
	printf("write test, write to file descriptor\n");
	write(fd,"writetest",10);
	char *str2=malloc(128);
	char *str21=malloc(128);
	read(fd,str2,13);
	printf("\nread: %s\n", str2);
	printf("close file decriptor test \n");
	close(fd);

	int a=unlink("rootfs/tobedel.txt");
	printf("unlinkres:%d\n",a );
	int a1=open("rootfs/tobedel.txt",2);
	printf("open after unlink:%d\n",a1 );
	read(a1,str21,10);
	printf("read: %s\n", str21);

	/*chdir(),getcwd() in pwd and cd*/

	/*getpid(),getppid()*/
	int pid=getpid();
	int ppid=getppid();
	printf("getpid: %d getppid: %d \n",pid,ppid);

	/*fork() wait() waitpid() */
	printf("fork test \n");
	int p=fork();
	if(p) {
		printf("parent \n");
       	waitpid(p,&status);
       	printf("parentend \n");
    }else {
        printf("child \n");
    }
    return 0;
}