// main.c - BSDSF21A033 Feature 1, 2, 3, and 4
// Author: Fahad Azeem (BSDSF21A033)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

#define MAXARGS 128
#define PROMPT "BSDSF21A033$ "

// ---- Function Prototypes ----
int parse_line(char *line, char **argv);
void run_command(char **argv);

// ---- Parse input line into arguments ----
int parse_line(char *line, char **argv) {
    int argc = 0;
    char *token = strtok(line, " \t");
    while (token != NULL && argc < MAXARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    return argc;
}

// ---- Execute external command ----
void run_command(char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        // child
        execvp(argv[0], argv);
        fprintf(stderr, "myshell: %s: %s\n", argv[0], strerror(errno));
        exit(127);
    } else {
        // parent waits
        int status;
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }
    }
}

int main(void) {
    char *line = NULL;
    char *argv[MAXARGS];

    while (1) {
        // --- Use GNU Readline for input with arrow-key history ---
        line = readline(PROMPT);
        if (!line) {     // Ctrl+D pressed
            printf("\n");
            break;
        }

        // skip leading spaces/tabs
        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;

        // ignore empty or comment lines
        if (*p == '\0' || *p == '#') {
            free(line);
            continue;
        }

        // ---- handle !n command for re-execution ----
        if (p[0] == '!' && isdigit((unsigned char)p[1])) {
            int n = atoi(p + 1);
            char *cmd = get_history_command(n);
            if (cmd == NULL) {
                fprintf(stderr, "No such history entry: %d\n", n);
                free(line);
                continue;
            }
            free(line);
            line = cmd;
            p = line;
            printf("Re-executing: %s\n", line);
        }

        // ---- Add command to both custom and readline history ----
        add_history_cmd(p);
        add_history(p);

        int argc = parse_line(p, argv);
        if (argc == 0) {
            free(line);
            continue;
        }

        // ---- Check for built-ins (cd, help, exit, history) ----
        if (handle_builtin(argv)) {
            free(line);
            continue;
        }

        // ---- Otherwise execute external command ----
        run_command(argv);
        free(line);
    }

    return 0;
}

