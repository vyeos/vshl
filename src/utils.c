#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

void get_current_dir_name(char *buffer, size_t size) {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    char *last_slash = strrchr(cwd, '/');
    if (last_slash) {
      strncpy(buffer, last_slash + 1, size - 1);
    } else {
      strncpy(buffer, cwd, size - 1);
    }
    buffer[size - 1] = '\0';
  } else {
    strncpy(buffer, "unknown", size - 1);
  }
}

void get_git_branch(char *buffer, size_t size) {
  FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
  if (fp == NULL) {
    buffer[0] = '\0';
    return;
  }

  if (fgets(buffer, size, fp) != NULL) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }
  } else {
    buffer[0] = '\0';
  }
  pclose(fp);
}
