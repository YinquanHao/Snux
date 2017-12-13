#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>


int main(){
	char *argv[] = {
    "argv1",
    "argv2",
    "argv3",
    "argv4",
    NULL,
};


char *envp[] = {
    "envp1",
    "envp2",
    "envp3",
    "envp4",
    NULL,
};


	int status=0;
	int k = fork();
	if(k) {
		printf("parent \n");
       	waitpid(2, &status);
    }else {
    	status = 10;
        printf("child \n");
        //waitpid(2, &status);
        execvpe("bin/ls",argv,envp);
    }
    printf("test11111111111111111111111111111111111111111111111111111111111111 %d",status);
    k = fork();
    if(k){
    	printf("yinquan\n");
    	waitpid(2,&status);
    }else{
    	printf("child11\n");
    	execvpe("bin/test1",argv,envp);
    }
    printf("thanks giving!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    k = fork();
    if(k){
    	printf("liwen\n");
    	waitpid(2,&status);
    }else{
    	printf("child22\n");
    	execvpe("bin/test1",argv,envp);
    }
    printf("christmas\n");


/*
printf("inside main \n");



	char* buf = (char*) malloc(50);
	uint64_t res = getcwd(buf,50);
	printf("%s\n",buf);

	chdir("rootfs");
	res = getcwd(buf,50);
	//printf("hellllllllllo !!%s\n",buf);
	
	chdir("rootfs/test");
	res = getcwd(buf,50);
	//printf("%s\n",buf);

	chdir("rootfs/bin");
	res = getcwd(buf,50);
	//printf("%s\n",buf);

	int fd = open("rootfs/test/hello.txt",2);
	char* buf1 = (char*) malloc(50);
	ssize_t size = read(fd,buf1,50);

	for(int i =0;i<100;i++){
		printf("%s",buf1);
	}

	int res1 = execvpe("bin/test1",argv,envp);

	//printf("%d\n", size);
	/*
	printf("%s",buf1);
	int clo  = close(fd);

	//printf("%d \n", clo);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);
	malloc(50);


	/*
	int fd = open("rootfs/test/hello.txt",2);
	char* buf = (char*) malloc(50);
	ssize_t size = read(fd,buf,50);
	printf("%d\n", size);
	printf("%s",buf);
	int clo  = close(fd);
	printf("%d \n", clo);
*/
	//malloc(sizeof(DIR));
	/*
	DIR* dir = opendir("rootfs/bin");
	printf("ppr");
	printf("hhh1 %d \n",(dir->fd));
	dirent* dirp = readdir(dir);
	printf("directory %s",dirp->d_name);
	int clodir  = closedir(dir);
	printf("clodir%d\n",clodir);
	*/
	//printf("directoty 's fd %d",dir->fd);

	/*
	int a =0;
	char b='t';
	char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	write(1, &b, 1);
	a=a+1;
	write(1,&buf,5);
	a=a+2;
	printf("%d",a);
	char* test = 0x00000FFF80001000;
	printf("%s",test);
	*/
	//char buf[10] = {'a', 'b', 'd', 0, 0, 0, 0, 0, 0, 0};	
	//write(1, &buf, 1);
	/*
	char* str;
	str = (char*) malloc(25);
	strcpy(str,"hello, world");
	//while(1);
	//str = "hello, world";
	//printf("%s\n", str);
	//free((void*)str);
	printf("123123213123123 %s \n",str);
	//free((void*)str);
	char* str1;
	str1 = (char*) malloc(25);
	strcpy(str1,"dlrow ,olleh");
	printf("this:%s \n",str);
	printf("this:%s \n",str1);
	char* str2;
	str2 = (char*) malloc(25);
	strcpy(str2,"qwueuiqwieu");
	printf("this:%s \n",str2);

	DIR* dir = opendir("/rootfs/bin");


	printf("reaches here\n");


	//printf("%s",dir->node->name);
*/
	while(1);
	return 1;
}