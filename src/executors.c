#include "../include/executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void execute_command(char **args) {
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

void execute_pipeline(char **args1, char **args2) {
    int pipefd[2]; // pipefd[0] read, pipefd[1] write
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return;
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
}
