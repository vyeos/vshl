#include "../include/parser.h"
#include <stdlib.h>
#include <string.h>

void parse_line(char *line, char **args) {
  int i = 0;
  char *token = strtok(line, " ");
  while (token != NULL) {
    args[i] = token;
    i++;
    token = strtok(NULL, " ");
  }
  args[i] = NULL;
}

int check_command_method(char **args, char ***args1, char ***args2) {
  *args1 = args;
  *args2 = NULL;

  for (int j = 0; args[j] != NULL; j++) {

    if (args[j + 1] == NULL)
      continue;

    if (strcmp(args[j], "|") == 0) {
      args[j] = NULL;
      *args2 = &args[j + 1];
      return 1; // Pipeline
    } else if (strcmp(args[j], ">") == 0) {
      args[j] = NULL;
      *args2 = &args[j + 1];
      return 2; // Output Redirection (overwrite)
    } else if (strcmp(args[j], ">>") == 0) {
      args[j] = NULL;
      *args2 = &args[j + 1];
      return 3; // Output Redirection (append)
    } else if (strcmp(args[j], "<") == 0) {
      args[j] = NULL;
      *args2 = &args[j + 1];
      return 4; // Input Redirection
    } else if (strcmp(args[j], "2>") == 0) {
      args[j] = NULL;
      *args2 = &args[j + 1];
      return 5; // Stderr Redirection
    }
  }
  return 0; // Normal
}
