#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "jobs.h"

int handle_export(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "vshl: expected argument to \"export\"\n");
    return 1;
  }
  // ask for confirmation

  char *in = args[1];
  char *equal_sign = strchr(in, '=');

  if (equal_sign == NULL) {
    fprintf(stderr, "vshl: invalid format. Use KEY=VALUE\n");
    return 1;
  }

  *equal_sign = '\0';
  char *key = in;
  char *val = equal_sign + 1;

  if (setenv(key, val, 1) != 0) {
    perror("vshl");
  }
  return 1;
}

int handle_builtin(char **args) {
  if (args[0] == NULL) {
    return 0;
  }

  if (strcmp(args[0], "exit") == 0) {
    return -1;
  }

  if (strcmp(args[0], "cd") == 0) {
    char *t_dir = args[1];
    if (!t_dir)
      t_dir = getenv("HOME");
    if (chdir(t_dir) == -1)
      perror("cd failed");
    return 1;
  }

  if (strcmp(args[0], "jobs") == 0) {
    print_jobs();
    return 1;
  }

  if (strcmp(args[0], "fg") == 0) {
    if (!args[1]) {
      printf("Usage: fg <job_id>\n");
      return 1;
    }
    int id = atoi(args[1]);
    Job *job = find_job_by_id(id);
    if (!job) {
      printf("fg: job not found: %s\n", args[1]);
      return 1;
    }

    if (job->status == JOB_STOPPED) {
      kill(job->pid, SIGCONT);
    }

    update_job_status(job->pid, JOB_RUNNING);

    printf("%s\n", job->command);

    int status;
    waitpid(job->pid, &status, WUNTRACED);

    if (WIFEXITED(status)) {
      remove_job(job->pid);
    } else if (WIFSIGNALED(status)) {
      if (WTERMSIG(status) != SIGINT)
        printf("\n");
      remove_job(job->pid);
    } else if (WIFSTOPPED(status)) {
      update_job_status(job->pid, JOB_STOPPED);
      printf("\n[Suspended] %d\n", job->pid);
    }
    return 1;
  }

  return 0;
}
