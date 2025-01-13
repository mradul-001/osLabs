// runcmd.c
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  pid_t cpid = fork();

  if (cpid < 0) {
    printf("Some error occurred!");
  }

  else if (cpid == 0) {
    // child process
    char *arguments[] = {argv[1], argv[2], NULL};
    execvp(arguments[0], arguments);
    exit(0);
  }

  else {
    wait(NULL);
    printf("Child finished!");
    exit(0);
  }

  return 0;
}