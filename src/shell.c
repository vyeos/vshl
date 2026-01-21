#include "shell.h"
#include "executor.h"
#include <string.h>

int launch_shell(char **args) {
  for (int i = 0; args[i] != NULL; i++) {
    // if (strcmp(args[i], "&&") == 0) {
    //   args[i] = NULL;
    //   char **next_cmd = &args[i + 1];
    //
    //   int status = execute_pipeline(args);
    //   if (status == 0)
    //     return launch_shell(next_cmd);
    //   return status;
    // } else if (strcmp(args[i], "||") == 0) {
    //   args[i] = NULL;
    //   char **next_cmd = &args[i + 1];
    //
    //   int status = execute_pipeline(args);
    //
    //   if (status != 0) {
    //     return launch_shell(next_cmd);
    //   }
    //   return status;
    // } else if (strcmp(args[i], ";") == 0) {
    //   args[i] = NULL;
    //   char **next_cmd = &args[i + 1];
    //
    //   execute_pipeline(args);
    //   return launch_shell(next_cmd);
    // }
  }
  // return execute_pipeline(args);
}
