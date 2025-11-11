// builtins.c - BSDSF21A033 Feature 2, 3, and 6
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "shell.h"

void print_help(void) {
    printf("Available built-in commands:\n");
    printf("  cd <dir>   - Change directory\n");
    printf("  help       - Show this help message\n");
    printf("  jobs       - List background jobs\n");
    printf("  history    - Show last 20 executed commands\n");
    printf("  exit       - Exit the shell\n");
}

int handle_builtin(char **args) {
    if (args == NULL || args[0] == NULL)
        return 0; // no command

    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    } else if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    } else if (strcmp(args[0], "jobs") == 0) {
        list_jobs();          // ✅ now shows real background jobs
        return 1;
    } else if (strcmp(args[0], "history") == 0) {
        show_history();
        return 1;
    }

    return 0; // not a built-in command
}

