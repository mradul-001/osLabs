#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main () {

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        printf("%s", "Pipe couldn't be opened!\n");
    }

    pid_t cpid = fork();

    if (cpid == -1) {
        printf("%s", "Error in forking the child!\n");
    }

    else if (cpid == 0) {
        // closing the write end of the pipe
        close(pipefd[1]);

        // read from the parent and write
        char *buff;
        while(read(pipefd[0], buff, 1)) {
            write(STDOUT_FILENO, buff, 1);
        }
        write(STDOUT_FILENO, "\n", 1);

        // close fd and exit
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    else {
        // close the read end of the parent
        close(pipefd[0]);

        char *buff = "Writing to the parent!\n";
        write(pipefd[1], buff, strlen(buff) + 1);

        close(pipefd[1]);
        wait(NULL);
        printf("Child exited\n");
    }

    

    return 0;
}