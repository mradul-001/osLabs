#include <stdio.h>
#include <unistd.h>

int main() {

  // this will get executed
  printf("Before exec syscall\n");

  char *argv[] = {"ls", "-l", NULL};
  execvp(argv[0], argv);

  // All statements are ignored after execvp() call as this whole
  //  process is replaced by another process
  printf("After exec syscall\n");

  return 0;
}