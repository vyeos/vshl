#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config/vshlrc.h"
#include "globbing.h"
#include "parser.h"
#include "shell.h"
#include "utils.h"

static void ensure_vshlrc_exists(const char *path) {
  if (path == NULL)
    return;

  struct stat st;
  if (stat(path, &st) == 0) {
    return;
  }

  FILE *fp = fopen(path, "w");
  if (fp == NULL) {
    return;
  }

  fprintf(fp, "# ~/.vshlrc\n");
  fprintf(fp, "# Lines here run when vshl starts (similar to ~/.zshrc).\n");
  fprintf(fp, "\n");
  fprintf(fp, "# Examples:\n");
  fprintf(fp, "# export PATH=\"$PATH:$HOME/bin\"\n");
  fprintf(fp, "# alias ll=\"ls -la\"\n");
  fclose(fp);
}

int source_vshlrc(void) {
  const char *home = getenv("HOME");
  if (home == NULL || home[0] == '\0') {
    return 0;
  }

  char path[1024];
  snprintf(path, sizeof(path), "%s/.vshlrc", home);
  ensure_vshlrc_exists(path);

  struct stat st;
  if (stat(path, &st) != 0) {
    return 0;
  }
  if (!S_ISREG(st.st_mode)) {
    return 0;
  }

  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    return 0;
  }

  char *line = NULL;
  size_t cap = 0;
  ssize_t nread;

  while ((nread = getline(&line, &cap, fp)) != -1) {
    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0';

    char *p = line;
    while (*p == ' ' || *p == '\t')
      p++;
    if (*p == '\0' || *p == '#')
      continue;

    char expanded_line[4096];
    expand_envs(p, expanded_line, sizeof(expanded_line));

    char *args[128];
    parse_line(expanded_line, args);
    if (args[0] == NULL)
      continue;

    int status = 0;
    char **expanded_globs_args = expand_globs(args);
    if (expanded_globs_args) {
      status = new_shell(expanded_globs_args);
      free_expanded_args(expanded_globs_args);
    } else {
      status = new_shell(args);
    }

    if (status == -1) {
      free(line);
      fclose(fp);
      return -1;
    }
  }

  free(line);
  fclose(fp);
  return 0;
}
