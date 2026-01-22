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

void expand_envs(char *in, char *out, size_t out_size) {
  size_t i = 0, j = 0;
  while (in[i] != '\0' && j < out_size - 1) {
    if ((i == 0 && in[i] == '~') || (in[i] == '~' && in[i - 1] == ' ')) {
      char *home = getenv("HOME");
      i++;
      if (home != NULL) {
        size_t len = strlen(home);
        if (j + len < out_size) {
          strncpy(out + j, home, len);
          j += len;
        }
      }
      continue;
    }

    if (in[i] == '$') {
      i++;
      char var_name[256];
      int k = 0;

      if (in[i] == '{') {
        i++;
        while (in[i] != '}' && in[i] != '\0') {
          if (k < 255)
            var_name[k++] = in[i];
          i++;
        }
        if (in[i] == '}')
          i++;
      } else {
        while (isalnum(in[i]) || in[i] == '_') {
          if (k < 255)
            var_name[k++] = in[i];
          i++;
        }
      }
      var_name[k] = '\0';

      char *value = getenv(var_name);
      if (value != NULL) {
        size_t len = strlen(value);
        if (j + len < out_size) {
          strncpy(out + j, value, len);
          j += len;
        }
      }
      continue;
    }
    out[j++] = in[i++];
  }
  out[j] = '\0';
}
