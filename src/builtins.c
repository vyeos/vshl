#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "alias.h"
#include "builtins.h"
#include "jobs.h"

int handle_export(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "vshl: expected argument to \"export\"\n");
    return 1;
  }

  int ret = 0;
  for (int i = 1; args[i] != NULL; i++) {
    char *in = args[i];
    char *equal_sign = strchr(in, '=');
    if (equal_sign == NULL) {
      fprintf(stderr, "vshl: invalid format. Use KEY=VALUE\n");
      ret = 1;
      continue;
    }

    *equal_sign = '\0';
    char *key = in;
    char *val = equal_sign + 1;

    if (key[0] == '\0') {
      fprintf(stderr, "vshl: invalid format. Use KEY=VALUE\n");
      ret = 1;
      continue;
    }

    if (setenv(key, val, 1) != 0) {
      perror("vshl");
      ret = 1;
    }
  }
  return ret;
}

int handle_unset(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "vshl: expected argument to \"unset\"\n");
    return 1;
  }

  int ret = 0;
  for (int i = 1; args[i] != NULL; i++) {
    if (unsetenv(args[i]) != 0) {
      perror("vshl");
      ret = 1;
    }
  }
  return ret;
}

static int handle_alias(char **args) {
  if (args[1] == NULL) {
    alias_print_all();
    return 0;
  }

  char *name = NULL;
  char *value = NULL;
  char value_buf[4096];

  char *eq = strchr(args[1], '=');
  if (eq != NULL) {
    *eq = '\0';
    name = args[1];
    value = eq + 1;
  } else {
    if (args[2] == NULL) {
      fprintf(stderr, "vshl: expected VALUE for \"alias\"\n");
      return 1;
    }
    name = args[1];

    value_buf[0] = '\0';
    for (int i = 2; args[i] != NULL; i++) {
      size_t remaining = sizeof(value_buf) - strlen(value_buf) - 1;
      if (remaining == 0)
        break;
      strncat(value_buf, args[i], remaining);
      if (args[i + 1] != NULL) {
        remaining = sizeof(value_buf) - strlen(value_buf) - 1;
        if (remaining == 0)
          break;
        strncat(value_buf, " ", remaining);
      }
    }
    value = value_buf;
  }

  if (name == NULL || name[0] == '\0') {
    fprintf(stderr, "vshl: invalid alias name\n");
    return 1;
  }

  if (value == NULL) {
    fprintf(stderr, "vshl: invalid alias value\n");
    return 1;
  }

  alias_set(name, value);
  return 0;
}

int handle_builtin(char **args, int *status_out) {
  if (args[0] == NULL) {
    return 0;
  }

  if (status_out == NULL) {
    return 0;
  }

  if (strcmp(args[0], "exit") == 0) {
    return -1;
  }

  if (strcmp(args[0], "cd") == 0) {
    char *t_dir = args[1];
    if (!t_dir)
      t_dir = getenv("HOME");
    if (chdir(t_dir) == -1) {
      perror("cd failed");
      *status_out = 1;
    } else {
      *status_out = 0;
    }
    return 1;
  }

  if (strcmp(args[0], "jobs") == 0) {
    print_jobs();
    *status_out = 0;
    return 1;
  }

  if (strcmp(args[0], "fg") == 0) {
    if (!args[1]) {
      printf("Usage: fg <job_id>\n");
      *status_out = 1;
      return 1;
    }
    int id = atoi(args[1]);
    Job *job = find_job_by_id(id);
    if (!job) {
      printf("fg: job not found: %s\n", args[1]);
      *status_out = 1;
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
      *status_out = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
      if (WTERMSIG(status) != SIGINT)
        printf("\n");
      remove_job(job->pid);
      *status_out = 128 + WTERMSIG(status);
    } else if (WIFSTOPPED(status)) {
      update_job_status(job->pid, JOB_STOPPED);
      printf("\n[Suspended] %d\n", job->pid);
      *status_out = 128 + WSTOPSIG(status);
    } else {
      *status_out = 1;
    }
    return 1;
  }

  if (strcmp(args[0], "export") == 0) {
    *status_out = handle_export(args);
    return 1;
  }

  if (strcmp(args[0], "unset") == 0) {
    *status_out = handle_unset(args);
    return 1;
  }

  if (strcmp(args[0], "alias") == 0) {
    *status_out = handle_alias(args);
    return 1;
  }

  return 0;
}
