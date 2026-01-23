#include <string.h>

#include "alias.h"
#include "builtins.h"
#include "executor.h"
#include "shell.h"

int new_shell(char **args) {
  if (args[0] == NULL)
    return 0;

  if (strcmp(args[0], "export") == 0) {
    handle_export(args);
    return 1;
  } else if (strcmp(args[0], "unset") == 0) {
    handle_unset(args);
    return 1;
  } else if (strcmp(args[0], "alias") == 0) {
    add_alias(args[1], args[2]);
    return 1;
  }

  int i = 0;
  while (args[i] != NULL) {
    if (strcmp(args[i], "&&") == 0) {
      args[i] = NULL;
      int status = run_command_unit(args);
      if (status == 0) {
        return new_shell(&args[i + 1]);
      }
      return status;
    } else if (strcmp(args[i], "||") == 0) {
      args[i] = NULL;
      int status = run_command_unit(args);
      if (status != 0) {
        return new_shell(&args[i + 1]);
      }
      return status;
    } else if (strcmp(args[i], ";") == 0) {
      args[i] = NULL;
      run_command_unit(args);
      return new_shell(&args[i + 1]);
    }
    i++;
  }

  return run_command_unit(args);
}
