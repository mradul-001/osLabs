#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t cpid = fork();
  if (cpid < 0) {
    printf("Some error occurred!");
  } else if (cpid == 0) {
    // child process
    printf("Executing the command.\n");
    char *myargs[] = {"ls", NULL};    // make the argument array
    execvp("ls", myargs);          // execute the executable
    exit(0);
  } else {
    // parent process
    wait(NULL);
    printf("Terminating the parent.");
    exit(0);
  }
  return 0;
}