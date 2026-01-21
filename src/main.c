#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "shell.h"
#include "utils.h"
#include "globbing.h"

int main() {
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  char *args[64];
  char dir_name[256];
  char git_branch[256];

  while (true) {
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
      printf("\nExiting...\n");
      break;
    }

    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0';

    if (line[0] == '\0')
      continue;

    parse_line(line, args);

    if (args[0] == NULL) {
      continue;
    }

    int status = 0;
    char **expanded_args = expand_globs(args);
    if (expanded_args) {
        status = new_shell(expanded_args);
        free_expanded_args(expanded_args);
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
