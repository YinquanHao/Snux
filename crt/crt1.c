#include <stdlib.h>
int main();
void _start(void) {
  // call main() and exit() here
	int res;
	res = main();
	//exit(res);


/*
	int argc=1;
	char **argv;
	char **envp;

	//reference:course hints---How Statically Linked Programs Run on Linux
	void *entry;
	__asm__ __volatile__(
		"movq %%rsp, %0"//%rsp is the stack pointer, 
						//which points to the top of the current stack frame
		:"=a" (entry)
	);
	
	argc = *((int *)entry + 2);// move addr of entry 2*int to point to argc
	argv =(char**)((int *)entry + 2)+1;//move to addr of argv
	envp = argv+argc + 1;

	int res;
	res = main(argc, argv, envp);

	exit(res);*/
 }
