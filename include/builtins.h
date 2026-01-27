#ifndef BUILTINS_H
#define BUILTINS_H

int handle_export(char **args);
int handle_unset(char **args);
int handle_builtin(char **args, int *status_out);

#endif
