#include <_string.h>
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

void list_alias() { return; }

void add_alias(char *name, char *cmd) {

  if (name == NULL || cmd == NULL) {
    fprintf(stderr, "vshl: expected 2 arguments for \"alias\"\n");
    return;
  }

  Alias *current = head;

  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      free(current->cmd);
      // what strdup does
      // new_node->name = malloc(strlen(name) + 1); // +1 for the \0 terminator
      // strcpy(new_node->name, name);
      current->cmd = strdup(cmd);
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

  new_node->next = head;
  head = new_node;
}

char get_alias(char *name) {
  if (name == NULL) {
    fprintf(stderr, "vshl: expected argument for \"alias\"\n");
    return 1;
  }

  Alias *current = head;

  while (current != NULL) {
    if (current->name == name) {
      return *current->name;
    }
  }
  fprintf(stderr, "vshl: no alias for \"%d\"\n", *name);
  return 1;
}
