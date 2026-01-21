#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

void parse_line(char *line, char **args);

int split_pipeline(char **args, char ***args1, char ***args2);

#endif