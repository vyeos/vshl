#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "executor.h"
#include "builtins.h"
#include "utils.h"

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[64];
    char dir_name[256];
    char git_branch[256];

    while (true) {
        get_current_dir_name(dir_name, sizeof(dir_name));
        get_git_branch(git_branch, sizeof(git_branch));
        
        if (strlen(git_branch) > 0) {
            printf("%s git:%s > ", dir_name, git_branch);
        } else {
            printf("%s > ", dir_name);
        }
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

        char **args1 = NULL;
        char **args2 = NULL;
        
        int method = check_method_and_split(args, &args1, &args2);

        switch (method) {
            case 0: // Normal
                execute_command(args);
                break;
            case 1: // Pipeline
                execute_pipeline(args1, args2);
                break;
            case 2: // >
                execute_redirection(args1, args2, 2);
                break;
            case 3: // >>
                execute_redirection(args1, args2, 3);
                break;
            case 4: // <
                execute_redirection(args1, args2, 4);
                break;
            case 5: // 2>
                execute_redirection(args1, args2, 5);
                break;
            default:
                printf("Unknown method\n");
                break;
        }
    }

    free(line);
    return 0;
}
