#include "../include/parser.h"
#include <string.h>
#include <stdlib.h>

void parse_line(char *line, char **args) {
    int i = 0;
    char *token = strtok(line, " ");
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

int split_pipeline(char **args, char ***args1, char ***args2) {
    *args1 = args;
    *args2 = NULL;

    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "|") == 0) {
            args[j] = NULL;
            *args2 = &args[j + 1];
            return 1;
        }
    }
    return 0;
}
