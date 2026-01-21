#include "globbing.h"
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char **expand_globs(char **args) {
    if (!args) return NULL;

    size_t capacity = 64;
    size_t count = 0;
    char **expanded = malloc(capacity * sizeof(char *));
    if (!expanded) {
        perror("malloc");
        return NULL;
    }

    for (int i = 0; args[i] != NULL; i++) {
        if (strpbrk(args[i], "*?") != NULL) {
            glob_t glob_result;
            int flags = GLOB_NOCHECK | GLOB_TILDE;
            
            if (glob(args[i], flags, NULL, &glob_result) == 0) {
                for (size_t j = 0; j < glob_result.gl_pathc; j++) {
                    if (count >= capacity - 1) {
                        capacity *= 2;
                        char **temp = realloc(expanded, capacity * sizeof(char *));
                        if (!temp) {
                            perror("realloc");
                            globfree(&glob_result);
                            free(expanded); 
                            return NULL;
                        }
                        expanded = temp;
                    }
                    expanded[count++] = strdup(glob_result.gl_pathv[j]);
                }
            } else {
                if (count >= capacity - 1) {
                        capacity *= 2;
                        expanded = realloc(expanded, capacity * sizeof(char *));
                }
                expanded[count++] = strdup(args[i]);
            }
            globfree(&glob_result);
        } else {
            if (count >= capacity - 1) {
                capacity *= 2;
                expanded = realloc(expanded, capacity * sizeof(char *));
            }
            expanded[count++] = strdup(args[i]);
        }
    }
    
    expanded[count] = NULL;
    return expanded;
}

void free_expanded_args(char **args) {
    if (!args) return;
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}
