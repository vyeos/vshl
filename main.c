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
  char *args[64];
  char *token;

  while (true) {
    printf("myshell> ");
    fflush(stdout);
    nread = getline(&line, &len, stdin);

    if (nread == -1) { // checking for \0
      printf("\nExiting...\n");
      break;
    }

    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0';

    if (line[0] == '\0')
      continue;

    if (strcmp(line, "exit") == 0) {
      break;
    }

    int i = 0;
    token = strtok(line, " ");
    while (token != NULL) {
      args[i] = token;
      i++;
      token = strtok(NULL, " "); // passing NULL as 1st arg to tell strtok to
                                 // take the previous var
    }

    args[i] =
        NULL; // set the last char as NULL so that execvp stops reading there

    if (strcmp(args[0], "cd") == 0) {
      char *home = getenv("HOME");
      char *t_dir = NULL;
      if (args[1] == NULL || strcmp(args[1], "~") == 0) {
        t_dir = home;
      } else {
        t_dir = args[1];
      }
      int result = chdir(t_dir);
      if (result == -1) {
        perror("cd failed");
      }
    } else {
      pid_t pid = fork();
      if (pid == 0) { // child
        execvp(args[0], args);
        perror("Error");
        exit(1);
      } else if (pid > 0) { // parent
        waitpid(pid, NULL, 0);
      } else {
        perror("Fork failed");
      }
    }
  }
  free(line);
  return 0;
}
