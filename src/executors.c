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

// Helper to execute a single logical unit (command or pipeline)
int run_command_unit(char **args) {
    if (args[0] == NULL) return 0;

    int builtin_status = handle_builtin(args);
    if (builtin_status == -1) return -1; // exit shell
    if (builtin_status == 1) return 0;   // success (builtin executed)

    char **args1 = NULL;
    char **args2 = NULL;
    int method = check_method_and_split(args, &args1, &args2);

    switch (method) {
        case 0: return execute_command(args);
        case 1: return execute_pipeline(args1, args2);
        case 2: return execute_redirection(args1, args2, 2); // >
        case 3: return execute_redirection(args1, args2, 3); // >>
        case 4: return execute_redirection(args1, args2, 4); // <
        case 5: return execute_redirection(args1, args2, 5); // 2>
        default:
            printf("Unknown method\n");
            return 1;
    }
}

// Recursive function to handle &&, ||, ;
int run_logic_chain(char **args) {
    if (args[0] == NULL) return 0;

    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "&&") == 0) {
            args[i] = NULL;
            int status = run_command_unit(args);
            if (status == 0) {
                return run_logic_chain(&args[i + 1]);
            }
            return status;
        } else if (strcmp(args[i], "||") == 0) {
            args[i] = NULL;
            int status = run_command_unit(args);
            if (status != 0) {
                return run_logic_chain(&args[i + 1]);
            }
            return status;
        } else if (strcmp(args[i], ";") == 0) {
            args[i] = NULL;
            run_command_unit(args);
            return run_logic_chain(&args[i + 1]);
        }
        i++;
    }

    return run_command_unit(args);
}

int execute_command(char **args) {
  pid_t pid = fork();
  if (pid == 0) { // child
    execvp(args[0], args);
    perror("Error");
    exit(1);
  } else if (pid > 0) { // parent
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
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
    execvp(args1[0], args1);
    perror("Exec failed");
    exit(1);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
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
  waitpid(pid1, &status1, 0);
  waitpid(pid2, &status2, 0);
  return WEXITSTATUS(status2); // Return status of the last command in pipeline
}
