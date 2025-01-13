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
    printf("PID of the child: %d\n", getpid());
    exit(0);
  } else {
    // parent process
    pid_t pid_of_terminated = wait(NULL);
    printf("PID of the child killed: %d\n", pid_of_terminated);
    exit(0);
  }
  return 0;
}