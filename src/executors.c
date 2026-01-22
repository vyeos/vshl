#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "executor.h"
#include "parser.h"
#include "signals.h"

int run_command_unit(char **args) {
  if (args[0] == NULL)
    return 0;

  int builtin_status = handle_builtin(args);
  if (builtin_status == -1)
    return -1;
  if (builtin_status == 1)
    return 0;

  char **args1 = NULL;
  char **args2 = NULL;
  int method = check_method_and_split(args, &args1, &args2);

  switch (method) {
  case 0:
    return execute_command(args);
  case 1:
    return execute_pipeline(args1, args2);
  case 2:
    return execute_redirection(args1, args2, 2); // >
  case 3:
    return execute_redirection(args1, args2, 3); // >>
  case 4:
    return execute_redirection(args1, args2, 4); // <
  case 5:
    return execute_redirection(args1, args2, 5); // 2>
  default:
    printf("Unknown method\n");
    return 1;
  }
}

int execute_command(char **args) {
  pid_t pid = fork();
  if (pid == 0) { // child
    restore_child_signals();
    execvp(args[0], args);
    perror("Error");
    exit(1);
  } else if (pid > 0) { // parent
    int status;
    waitpid(pid, &status, WUNTRACED);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) != SIGINT)
             printf("\n");
        return 128 + WTERMSIG(status);
    }
    if (WIFSTOPPED(status)) {
        printf("\n[Suspended] %d\n", pid);
        return 128 + WSTOPSIG(status);
    }
    return 1;
  } else {
    perror("Fork failed");
    return 1;
  }
}

int execute_redirection(char **args1, char **args2, int mode) {
  pid_t pid = fork();
  if (pid == 0) { // child process
    int fd = -1;
    if (mode == 2) { // >
      fd = open(args2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
        perror("Open failed");
        exit(1);
      }
      dup2(fd, STDOUT_FILENO);
    } else if (mode == 3) { // >>
      fd = open(args2[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
      if (fd == -1) {
        perror("Open failed");
        exit(1);
      }
      dup2(fd, STDOUT_FILENO);
    } else if (mode == 4) { // <
      fd = open(args2[0], O_RDONLY);
      if (fd == -1) {
        perror("Open failed");
        exit(1);
      }
      dup2(fd, STDIN_FILENO);
    } else if (mode == 5) { // 2>
      fd = open(args2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
        perror("Open failed");
        exit(1);
      }
      dup2(fd, STDERR_FILENO);
    }

    close(fd);

    // execute args1
    restore_child_signals();
    execvp(args1[0], args1);
    perror("Exec failed");
    exit(1);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, WUNTRACED);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    if (WIFSTOPPED(status)) {
        printf("\n[Suspended] %d\n", pid);
        return 128 + WSTOPSIG(status);
    }
    return 1;
  } else {
    perror("Fork failed");
    return 1;
  }
}

int execute_pipeline(char **args1, char **args2) {
  int pipefd[2]; // pipefd[0] read, pipefd[1] write
  if (pipe(pipefd) == -1) {
    perror("Pipe failed");
    return 1;
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
  int status1, status2;
  waitpid(pid1, &status1, WUNTRACED);
  waitpid(pid2, &status2, WUNTRACED);
  
    if (WIFEXITED(status2)) {
        return WEXITSTATUS(status2);
    }
    if (WIFSIGNALED(status2)) {
        return 128 + WTERMSIG(status2);
    }
    if (WIFSTOPPED(status2)) {
        printf("\n[Suspended] %d\n", pid2);
        return 128 + WSTOPSIG(status2);
    }
    return 1;
}
