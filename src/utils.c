#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
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

void expand_envs(char *in, char *out) {
  int i = 0, j = 0;
  while (in[i] != 0) {
    if ((i == 0 && in[i] == '~') || (in[i] == '~' && in[i - 1] == ' ')) {
      char *tilde = getenv("HOME");
      i++;
      if (tilde != NULL) {
        for (int m = 0; tilde[m] != '\0'; m++) {
          out[j] = tilde[m];
          j++;
        }
      }
    } else if (in[i] != '$') {
      out[j] = in[i];
      j++;
      i++;
    } else {
      i++;
      char var_name[256];
      int k = 0;
      while (isalnum(in[i]) || in[i] == '_') {
        var_name[k] = in[i];
        k++;
        i++;
      }
      var_name[k] = '\0';

      char *value = getenv(var_name);
      if (value != NULL) {
        for (int m = 0; value[m] != '\0'; m++) {
          out[j] = value[m];
          j++;
        }
      }
    }
  }
  out[j] = '\0';
}
