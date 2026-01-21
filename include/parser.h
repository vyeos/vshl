#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

void parse_line(char *line, char **args);

int check_command_method(char **args, char ***args1, char ***args2);

#endif
