#ifndef EXECUTOR_H
#define EXECUTOR_H

void execute_command(char **args);
void execute_pipeline(char **args1, char **args2);
void execute_redirection(char **args1, char **args2, int mode);

#endif
