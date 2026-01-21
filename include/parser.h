#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

void parse_line(char *line, char **args);

int check_method_and_split(char **args, char ***args1, char ***args2);

#endif
