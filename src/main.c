#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "executor.h"
#include "builtins.h"

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[64];

    while (true) {
        printf("vshl> ");
        fflush(stdout);
        nread = getline(&line, &len, stdin);

        if (nread == -1) {
            printf("\nExiting...\n");
            break;
        }

        if (nread > 0 && line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        if (line[0] == '\0')
            continue;
        
        parse_line(line, args);

        if (args[0] == NULL) {
            continue;
        }

        int builtin_status = handle_builtin(args);
        if (builtin_status == -1) {
            break; // exit
        } 
        if (builtin_status == 1) {
            continue; // executed builtin
        }

        // Check for pipeline
        char **args1 = NULL;
        char **args2 = NULL;
        
        // args is modified in place if pipe found
        if (split_pipeline(args, &args1, &args2)) {
            execute_pipeline(args1, args2);
        } else {
            execute_command(args);
        }
    }

    free(line);
    return 0;
}
