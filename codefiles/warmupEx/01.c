#include "stdio.h"
#include "stdlib.h"
#include <sys/wait.h>
#include <unistd.h>

int main() {

  pid_t rc = fork();

  if (rc < 0) {
    printf("Some error occured!");
  } else if (rc == 0) {
    // child process
    printf("I am child!\n");
    exit(0);
  } else {
    // the parent will wait until the child is terminated
    wait(NULL);
    printf("I am parent!\n");
  }

  return 0;
}