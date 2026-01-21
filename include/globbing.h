#ifndef GLOBBING_H
#define GLOBBING_H

char **expand_globs(char **args);

void free_expanded_args(char **args);

#endif