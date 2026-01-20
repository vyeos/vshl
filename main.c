#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // mem alloc
#include <string.h>
#include <sys/_types/_pid_t.h>
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

    char **args1 = args; // head
    char **args2 = NULL; // tail

    for (int j = 0; args[j] != NULL; j++) {
      if (strcmp(args[j], "|") == 0) {
        args[j] = NULL;
        args2 = &args[j + 1];
        break;
      }
    }

    if (args2 != NULL) { // pipeline found
      int pipefd[2];     // pipefd[0] read, pipefd[1] write
      if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        continue;
      }

      pid_t pid1 = fork();
      if (pid1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); // replace stdout with write end
        close(pipefd[1]);
        execvp(args1[0], args1);
        perror("exec 1 failed");
        exit(1);
      }

      pid_t pid2 = fork();
      if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); // replace stdin with read
        close(pipefd[0]);
        execvp(args2[0], args2);
        perror("exec 2 failed");
        exit(1);
      }

      close(pipefd[0]);
      close(pipefd[1]);
      waitpid(pid1, NULL, 0);
      waitpid(pid2, NULL, 0);
    } else {

      if (strcmp(args[0], "cd") == 0) {
        char *t_dir = args[1];
        if (!t_dir)
          t_dir = getenv("HOME");
        if (chdir(t_dir) == -1)
          perror("cd failed");
        continue;
      }

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
