#ifndef EXECUTOR_H
#define EXECUTOR_H

int execute_command(char **args);
int execute_pipeline(char **args1, char **args2);
int execute_redirection(char **args1, char **args2, int mode);

int run_command_unit(char **args);

#endif
