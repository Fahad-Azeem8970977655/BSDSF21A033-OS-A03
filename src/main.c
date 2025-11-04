// Simple shell for Feature 1 (Base shell)
// Author: BSDSF21A033
#include <ctype.h>
#include <ctype.h>
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXLINE 1024
#define MAXARGS 128
#define PROMPT "BSDSF21A033$ "  // prompt uses your roll number

// prototype
char *read_line(void);
int parse_line(char *line, char **argv);
void run_command(char **argv);

int main(void) {
    char *line;
    char *argv[MAXARGS];

    while (1) {
        // print prompt
        if (isatty(STDIN_FILENO)) {
            printf("%s", PROMPT);
            fflush(stdout);
        }

        line = read_line();
        if (line == NULL) {
            // EOF (Ctrl-D) — exit gracefully
            printf("\n");
            break;
        }

        // strip trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';

        // skip empty lines
                // skip empty or comment lines
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#') { free(line); continue; }

        // handle !n command (re-execute history)
        if (p[0] == '!' && isdigit(p[1])) {
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

        // add to history
        add_history_cmd(p);

        int argc = parse_line(p, argv);

        
        

        
  

        if (argc == 0) { free(line); continue; }

        // check for builtin "exit"
        // check built-ins

       if (handle_builtin(argv)) {
       free(line);
       continue;  // built-in handled
     }


       run_command(argv);

        free(line);
    }

    return 0;
}

// read a line from stdin, return malloc'd string (caller frees).
char *read_line(void) {
    char *buf = malloc(MAXLINE);
    if (!buf) return NULL;
    if (fgets(buf, MAXLINE, stdin) == NULL) {
        free(buf);
        return NULL;
    }
    return buf;
}

// very simple tokenizer: splits by whitespace, supports up to MAXARGS-1 args
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

// fork + exec external command
void run_command(char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        // child
        execvp(argv[0], argv);
        // only reached on error
        fprintf(stderr, "myshell: %s: %s\n", argv[0], strerror(errno));
        exit(127);
    } else {
        // parent: wait for child
        int status;
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }
    }
}
