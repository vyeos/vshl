#include "parser.h"
#include <stdlib.h>
#include <string.h>

void parse_line(char *line, char **args) {
  if (line == NULL || args == NULL) {
    if (args != NULL) args[0] = NULL;
    return;
  }
  
  int i = 0;
  int pos = 0;
  char *current_token = line;
  int in_quotes = 0;
  int token_start = 1;

  while (line[pos] != '\0') {
    if (line[pos] == '\\') {
       memmove(&line[pos], &line[pos + 1], strlen(&line[pos + 1]) + 1);
       if (line[pos] != '\0') {
           pos++;
       }
       continue;
    }

    if (line[pos] == '"') {
      in_quotes = !in_quotes;
      memmove(&line[pos], &line[pos + 1], strlen(&line[pos + 1]) + 1);
      continue;
    }
    if (line[pos] == ' ' && !in_quotes) {
      line[pos] = '\0';
      if (!token_start) {
        args[i++] = current_token;
        token_start = 1;
      }
    } else {
      if (token_start) {
        current_token = &line[pos];
        token_start = 0;
      }
    }
    pos++;
  }

  if (!token_start) {
    args[i++] = current_token;
  }
  args[i] = NULL;
}

int check_method_and_split(char **args, char ***args1, char ***args2) {
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
