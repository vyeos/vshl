#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line_editing.h"

#if defined(__has_include)
#if __has_include(<editline/readline.h>)
#define VSHL_USE_READLINE_COMPAT 1
#include <editline/readline.h>
#include <readline/history.h>
#elif __has_include(<readline/readline.h>)
#define VSHL_USE_READLINE_COMPAT 1
#include <readline/readline.h>
#include <readline/history.h>
#endif
#endif

#ifndef VSHL_USE_READLINE_COMPAT
#define VSHL_USE_READLINE_COMPAT 0
#endif

#if VSHL_USE_READLINE_COMPAT

static char g_history_path[1024];

static char **g_cmds = NULL;
static size_t g_cmds_count = 0;
static size_t g_cmds_cap = 0;

static int cmd_exists(const char *cmd) {
  for (size_t i = 0; i < g_cmds_count; i++) {
    if (strcmp(g_cmds[i], cmd) == 0)
      return 1;
  }
  return 0;
}

static void cmd_add(const char *cmd) {
  if (cmd == NULL || cmd[0] == '\0')
    return;
  if (cmd_exists(cmd))
    return;

  if (g_cmds_count == g_cmds_cap) {
    size_t new_cap = (g_cmds_cap == 0) ? 64 : g_cmds_cap * 2;
    char **next = realloc(g_cmds, new_cap * sizeof(*g_cmds));
    if (next == NULL)
      return;
    g_cmds = next;
    g_cmds_cap = new_cap;
  }

  g_cmds[g_cmds_count++] = strdup(cmd);
}

static const char *extract_first_word(const char *line, char *out, size_t out_size) {
  if (line == NULL || out == NULL || out_size == 0)
    return NULL;

  while (*line && isspace((unsigned char)*line))
    line++;
  if (*line == '\0')
    return NULL;

  size_t i = 0;
  while (line[i] && !isspace((unsigned char)line[i])) {
    if (i + 1 >= out_size)
      break;
    out[i] = line[i];
    i++;
  }
  out[i] = '\0';
  return out[0] ? out : NULL;
}

static char *cmd_history_generator(const char *text, int state) {
  static size_t idx;
  if (state == 0)
    idx = 0;

  size_t text_len = strlen(text);
  while (idx < g_cmds_count) {
    const char *candidate = g_cmds[idx++];
    if (strncmp(candidate, text, text_len) == 0)
      return strdup(candidate);
  }
  return NULL;
}

static char **vshl_completion(const char *text, int start, int end) {
  (void)end;

  if (start == 0) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, cmd_history_generator);
  }

  return rl_completion_matches(text, rl_filename_completion_function);
}

void line_editing_init(void) {
  const char *home = getenv("HOME");
  if (home != NULL && home[0] != '\0') {
    snprintf(g_history_path, sizeof(g_history_path), "%s/.vshl_history", home);
    using_history();
    (void)read_history(g_history_path);
  }

  rl_attempted_completion_function = vshl_completion;
}

void line_editing_shutdown(void) {
  if (g_history_path[0] != '\0') {
    (void)write_history(g_history_path);
  }

  for (size_t i = 0; i < g_cmds_count; i++) {
    free(g_cmds[i]);
  }
  free(g_cmds);
  g_cmds = NULL;
  g_cmds_count = 0;
  g_cmds_cap = 0;
}

char *read_command_line(const char *prompt) {
  return readline(prompt);
}

void line_editing_record_line(const char *line) {
  if (line == NULL)
    return;

  if (line[0] != '\0') {
    add_history(line);
  }

  char cmd[256];
  if (extract_first_word(line, cmd, sizeof(cmd)) != NULL) {
    cmd_add(cmd);
  }
}

#else

void line_editing_init(void) {}
void line_editing_shutdown(void) {}

char *read_command_line(const char *prompt) {
  if (prompt != NULL) {
    fputs(prompt, stdout);
    fflush(stdout);
  }

  char *line = NULL;
  size_t cap = 0;
  ssize_t nread = getline(&line, &cap, stdin);
  if (nread == -1) {
    free(line);
    return NULL;
  }

  if (nread > 0 && line[nread - 1] == '\n')
    line[nread - 1] = '\0';

  return line;
}

void line_editing_record_line(const char *line) { (void)line; }

#endif