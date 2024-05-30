#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int status;
  int childProgs = 1;
  pid_t pid;
  printf("Creating child process 1.\n");
  pid = fork();
  if (pid == 0) {
    printf("Child process 1 transferring control to cat.\n");
    execlp("cat", "cat", "file.txt", NULL);
  } else if (pid > 0) {
    waitpid(pid, &status, 0);
    print_status(status, &childProgs);
    printf("Creating child process 2.\n");
    pid = fork();
    if (pid == 0) {
      printf("Child process 2 transferring control to abort.\n");
      abort();
    } else if (pid > 0) {
      waitpid(pid, &status, 0);
      print_status(status, &childProgs);
      printf("Creating child process 3.\n");
      pid = fork();
      if (pid == 0) {
        printf("Child process 3 transferring control to exit.\n");
        exit(0);
      } else if (pid > 0) {
        waitpid(pid, &status, 0);
        print_status(status, &childProgs);
        printf("Creating child process 4.\n");
        pid = fork();
        if (pid == 0) {
          printf("Child process 4 transferring control to sleep.\n");
          sleep(2);
          exit(0);
        } else if (pid > 0) {
          waitpid(pid, &status, 0);
          print_status(status, &childProgs);
        }
      }
    }
  }

  return 0;
}

void print_status(int status, int *a) {
  if (WIFEXITED(status)) {
    printf("Child %d exited normally with status code %d.\n", *a, WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child %d abnormally terminated with signal %d.\n", *a, WTERMSIG(status));
  } else {
    printf("Child exited for an unknown reason.\n");
  }

  *a = (*a) + 1;
}
