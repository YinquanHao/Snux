#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]) {
    pid_t pid;
    int status;
    while (1) {
        status = 0;
        pid = fork();
        if (pid > 0) {
            //change the process
            waitpid(pid, &status);
            if (status == -1) {
                printf("shell killed by illegal memory access!\n");
            }
            printf("initialize the OS complete");
        } else if (pid == 0) {
            char *argv[] = {
                "argv1",
                "argv2",
                "argv3",
                 NULL
            };


            char *envp[] = {
                "envp1",
                "envp2",
                "envp3",
                 NULL
            };
            execvpe("bin/test", argv, envp);
        } else {
            printf("error in fork!\n");
        }
    }
    return 0;
}
