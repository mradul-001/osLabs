#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int sig) { printf("I am not gonna stop!\n"); }

int main() {
  signal(SIGINT, handler);
  while (1) {
    sleep(1);
  }
}