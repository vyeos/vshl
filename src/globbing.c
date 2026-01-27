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
                            for (size_t k = 0; k < count; k++) free(expanded[k]);
                            free(expanded); 
                            return NULL;
                        }
                        expanded = temp;
                    }
                    char *dup = strdup(glob_result.gl_pathv[j]);
                    if (!dup) {
                        perror("strdup");
                        globfree(&glob_result);
                        for (size_t k = 0; k < count; k++) free(expanded[k]);
                        free(expanded);
                        return NULL;
                    }
                    expanded[count++] = dup;
                }
            } else {
                if (count >= capacity - 1) {
                        capacity *= 2;
                        char **temp = realloc(expanded, capacity * sizeof(char *));
                        if (!temp) {
                            perror("realloc");
                            for (size_t k = 0; k < count; k++) free(expanded[k]);
                            free(expanded);
                            return NULL;
                        }
                        expanded = temp;
                }
                char *dup = strdup(args[i]);
                if (!dup) {
                    perror("strdup");
                    for (size_t k = 0; k < count; k++) free(expanded[k]);
                    free(expanded);
                    return NULL;
                }
                expanded[count++] = dup;
            }
            globfree(&glob_result);
        } else {
            if (count >= capacity - 1) {
                capacity *= 2;
                char **temp = realloc(expanded, capacity * sizeof(char *));
                if (!temp) {
                    perror("realloc");
                    for (size_t k = 0; k < count; k++) free(expanded[k]);
                    free(expanded);
                    return NULL;
                }
                expanded = temp;
            }
            char *dup = strdup(args[i]);
            if (!dup) {
                perror("strdup");
                for (size_t k = 0; k < count; k++) free(expanded[k]);
                free(expanded);
                return NULL;
            }
            expanded[count++] = dup;
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
