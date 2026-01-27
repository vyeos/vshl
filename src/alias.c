#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alias.h"

typedef struct Alias {
  char *name;
  char *cmd;
  struct Alias *next;
} Alias;

Alias *head = NULL;

void alias_print_all(void) {
  for (Alias *current = head; current != NULL; current = current->next) {
    printf("alias %s=\"%s\"\n", current->name, current->cmd);
  }
}

void alias_set(const char *name, const char *cmd) {

  if (name == NULL || cmd == NULL) {
    fprintf(stderr, "vshl: expected 2 arguments for \"alias\"\n");
    return;
  }

  Alias *current = head;

  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      char *new_cmd = strdup(cmd);
      if (new_cmd == NULL) {
        perror("strdup failed");
        return;
      }
      free(current->cmd);
      current->cmd = new_cmd;
      return;
    }
    current = current->next;
  }

  Alias *new_node = malloc(sizeof(Alias));
  if (new_node == NULL) {
    perror("malloc failed");
    return;
  }

  new_node->name = strdup(name);
  new_node->cmd = strdup(cmd);
  if (new_node->name == NULL || new_node->cmd == NULL) {
    perror("strdup failed");
    free(new_node->name);
    free(new_node->cmd);
    free(new_node);
    return;
  }

  new_node->next = head;
  head = new_node;
}

const char *alias_get(const char *name) {
  if (name == NULL) {
    return NULL;
  }

  for (Alias *current = head; current != NULL; current = current->next) {
    if (strcmp(current->name, name) == 0) {
      return current->cmd;
    }
  }
  return NULL;
}

int alias_print_one(const char *name) {
  if (name == NULL || name[0] == '\0') {
    fprintf(stderr, "vshl: expected argument for \"alias\"\n");
    return 1;
  }

  const char *cmd = alias_get(name);
  if (cmd == NULL) {
    fprintf(stderr, "vshl: no alias for \"%s\"\n", name);
    return 1;
  }

  printf("alias %s=\"%s\"\n", name, cmd);
  return 0;
}

int alias_unset(const char *name) {
  if (name == NULL || name[0] == '\0') {
    fprintf(stderr, "vshl: expected argument for \"unalias\"\n");
    return 1;
  }

  Alias *prev = NULL;
  Alias *current = head;

  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      if (prev == NULL) {
        head = current->next;
      } else {
        prev->next = current->next;
      }
      free(current->name);
      free(current->cmd);
      free(current);
      return 0;
    }
    prev = current;
    current = current->next;
  }

  fprintf(stderr, "vshl: no alias for \"%s\"\n", name);
  return 1;
}
