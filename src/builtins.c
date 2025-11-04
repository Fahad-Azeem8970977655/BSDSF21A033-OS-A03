// builtins.c - BSDSF21A033 Feature 2
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "shell.h"

// Print list of built-in commands
void print_help(void) {
    printf("Available built-in commands:\n");
    printf("  cd <dir>   - Change directory\n");
    printf("  help       - Show this help message\n");
    printf("  jobs       - List background jobs (not yet implemented)\n");
    printf("  exit       - Exit the shell\n");
}

int handle_builtin(char **args) {
    if (args == NULL || args[0] == NULL)
        return 0; // no command

    // exit
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    // cd
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    }
    // help
    else if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    }
    // jobs (placeholder)
    else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not implemented yet.\n");
        return 1;
    }

    return 0; // not a built-in command
}
