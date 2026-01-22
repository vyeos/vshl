#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globbing.h"
#include "jobs.h"
#include "parser.h"
#include "shell.h"
#include "signals.h"
#include "utils.h"

int main() {
  setup_parent_signals();
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  char *args[64];
  char dir_name[256];
  char git_branch[256];

  while (true) {
    check_zombies();

    get_current_dir_name(dir_name, sizeof(dir_name));
    get_git_branch(git_branch, sizeof(git_branch));

    if (strlen(git_branch) > 0) {
      printf("%s git:(%s) > ", dir_name, git_branch);
    } else {
      printf("%s > ", dir_name);
    }

    fflush(stdout);
    nread = getline(&line, &len, stdin);

    if (nread == -1) {
      if (errno == EINTR) {
        clearerr(stdin);
        errno = 0;
        continue;
      }
      printf("\nExiting...\n");
      break;
    }

    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0';

    if (line[0] == '\0')
      continue;

    char expanded_line[4096];
    expand_envs(line, expanded_line, sizeof(expanded_line));

    parse_line(expanded_line, args);

    if (args[0] == NULL) {
      continue;
    }

    int status = 0;
    char **expanded_globs_args = expand_globs(args);
    if (expanded_globs_args) {
      status = new_shell(expanded_globs_args);
      free_expanded_args(expanded_globs_args);
    } else {
      status = new_shell(args);
    }

    if (status == -1) {
      break;
    }
  }

  free(line);
  return 0;
}
