#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]);
void _start(void) {
  // call main() and exit() here


	/*
	int res;
	res = main();
	exit(res);
	*/
	//initialize the argument count
	int argc=1;
	//argument value pointers
	char **argv;
	//enviroment argument pointers
	char **envp;
	//initialize the entry
	void *entry;
	//move the stack pointer to entry
	__asm__ __volatile__(
		"movq %%rsp, %0"
		:"=a" (entry)
	);
	//check argc
	argc = *((int *)entry + 2);// move addr of entry 2*int to point to argc
	//get argv address
	argv =(char**)((int *)entry + 2)+1;//move to addr of argv
	//get enviroment pointers address
	envp = argv+argc + 1;
	//initialize the res
	int res;
	//start the main function
	res = main(argc, argv, envp);
	//exits
	exit(res);	
 }
