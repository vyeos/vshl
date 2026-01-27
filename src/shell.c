#include <string.h>
#include <stdio.h>

#include "alias.h"
#include "executor.h"
#include "parser.h"
#include "shell.h"
#include "utils.h"

static int should_expand_alias(const char *cmd) {
  if (cmd == NULL)
    return 0;
  if (strcmp(cmd, "alias") == 0)
    return 0;
  if (strcmp(cmd, "unalias") == 0)
    return 0;
  if (strcmp(cmd, "export") == 0)
    return 0;
  if (strcmp(cmd, "unset") == 0)
    return 0;
  return 1;
}

static int new_shell_depth(char **args, int depth);

static int expand_first_word_alias(char **args, int depth) {
  if (depth > 10)
    return 0;
  if (!should_expand_alias(args[0]))
    return 0;

  const char *alias_cmd = alias_get(args[0]);
  if (alias_cmd == NULL)
    return 0;

  char combined[4096];
  combined[0] = '\0';
  strncat(combined, alias_cmd, sizeof(combined) - 1);

  for (int i = 1; args[i] != NULL; i++) {
    size_t remaining = sizeof(combined) - strlen(combined) - 1;
    if (remaining == 0)
      break;
    strncat(combined, " ", remaining);
    remaining = sizeof(combined) - strlen(combined) - 1;
    if (remaining == 0)
      break;
    strncat(combined, args[i], remaining);
  }

  char expanded[4096];
  expand_envs(combined, expanded, sizeof(expanded));

  char *alias_args[128];
  parse_line(expanded, alias_args);
  if (alias_args[0] == NULL)
    return 0;
  return new_shell_depth(alias_args, depth + 1);
}

static int new_shell_depth(char **args, int depth) {
  if (args[0] == NULL)
    return 0;

  if (depth > 20) {
    fprintf(stderr, "vshl: maximum recursion depth exceeded\n");
    return 1;
  }

  if (should_expand_alias(args[0])) {
    const char *alias_cmd = alias_get(args[0]);
    if (alias_cmd != NULL) {
      return expand_first_word_alias(args, depth);
    }
  }

  int i = 0;
  while (args[i] != NULL) {
    if (strcmp(args[i], "&&") == 0) {
      args[i] = NULL;
      int status = run_command_unit(args);
      if (status == 0) {
        return new_shell_depth(&args[i + 1], depth);
      }
      return status;
    } else if (strcmp(args[i], "||") == 0) {
      args[i] = NULL;
      int status = run_command_unit(args);
      if (status != 0) {
        return new_shell_depth(&args[i + 1], depth);
      }
      return status;
    } else if (strcmp(args[i], ";") == 0) {
      args[i] = NULL;
      run_command_unit(args);
      return new_shell_depth(&args[i + 1], depth);
    }
    i++;
  }

  return run_command_unit(args);
}

int new_shell(char **args) { return new_shell_depth(args, 0); }
