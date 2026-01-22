#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void get_current_dir_name(char *buffer, size_t size);
void get_git_branch(char *buffer, size_t size);
void expand_envs(char *in, char *out);

#endif
