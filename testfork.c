#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILD_COUNT 2

void f1(int);
void f2(int, int);

void f1(int x) {
  int local_int = x - 1;
  if (x > 1) {
    f2(local_int, local_int);
  }
}

void f2(int x, int y) {
  int local_int = x - 1;
  if (x > 1) {
    f1(local_int);
  }
}

int main(void) {
  pid_t main_pid = getpid(); // save PID

  for (int i = 0; i < CHILD_COUNT; i++) { // fork multiple children
    if (fork() == 0) { // child task
      f1(5);
      break;
    }
  }

  if (getpid() == main_pid) { // wait for all children
    for (int i = 0; i < CHILD_COUNT; i++) {
      wait(NULL);
    }
  }

  return 0;
}