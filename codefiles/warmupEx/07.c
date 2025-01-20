#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t cpid = fork();
  if (cpid < 0) {
    printf("Some error occurred!");
  } else if (cpid == 0) {
    printf("I am the child process.\n");
    // running the sleep command
    char *args[] = {"sleep", "100", NULL};
    execvp("sleep", args);
  } else {
    // kill the child process
    kill(cpid, SIGINT);
    // reap the child process
    waitpid(cpid, NULL, 0);
    printf("Killed the child.\n");
    exit(0);
  }
  return 0;
}