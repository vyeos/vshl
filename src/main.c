#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config/vshlrc.h"
#include "globbing.h"
#include "jobs.h"
#include "line_editing.h"
#include "parser.h"
#include "shell.h"
#include "signals.h"
#include "utils.h"

int main() {
  setup_parent_signals();
  char *args[64];
  char dir_name[256];
  char git_branch[256];

  line_editing_init();

  if (source_vshlrc() == -1) {
    line_editing_shutdown();
    return 0;
  }

  while (true) {
    check_zombies();

    get_current_dir_name(dir_name, sizeof(dir_name));
    get_git_branch(git_branch, sizeof(git_branch));

    char prompt[1024];
    if (strlen(git_branch) > 0) {
      snprintf(prompt, sizeof(prompt), "%s git:(%s) > ", dir_name, git_branch);
    } else {
      snprintf(prompt, sizeof(prompt), "%s > ", dir_name);
    }

    char *line = read_command_line(prompt);
    if (line == NULL) {
      printf("\nExiting...\n");
      break;
    }

    line_editing_record_line(line);

    if (line[0] == '\0')
    {
      free(line);
      continue;
    }

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
      free(line);
      break;
    }

    free(line);
  }

  line_editing_shutdown();
  return 0;
}
