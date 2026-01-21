#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

  return 0;
}
