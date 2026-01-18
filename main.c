#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // mem alloc
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // fork, exec, pid_t

int main() {
  char *line = NULL;
  size_t len = 0; // initial len -> will increase while typing
  ssize_t nread;

  while (true) {
    printf("myshell> ");
    fflush(stdout);
    nread = getline(&line, &len, stdin);

    if (nread == -1) { // checking for \0
      printf("\nExiting...\n");
      break;
    }

    if (nread > 0 && line[nread - 1] == '\n') {
      line[nread - 1] = '\0';
    }

    if (strcmp(line, "exit") == 0) {
      break;
    }

    // write future code here
  }
  free(line);
  return 0;
}
