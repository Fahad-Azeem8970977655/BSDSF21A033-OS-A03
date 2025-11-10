// main.c - BSDSF21A033 Features 1-5 (Base, Builtins, History, Readline, I/O redirection & single pipe)
// Author: Fahad Azeem (BSDSF21A033)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

#define MAXARGS 128
#define PROMPT "BSDSF21A033$ "

// parse a simple command (no pipe). Detects argv tokens and optional infile/outfile.
// argv[] will be filled (null-terminated). Caller must not free argv elements (they point into cmdline).
// Returns 0 on success.
static int parse_simple_command(char *cmdline, char **argv, char **infile, char **outfile) {
    *infile = NULL;
    *outfile = NULL;
    int argc = 0;
    char *tok = strtok(cmdline, " \t\n");
    while (tok != NULL && argc < MAXARGS - 1) {
        if (strcmp(tok, "<") == 0) {
            tok = strtok(NULL, " \t\n");
            if (!tok) { fprintf(stderr, "syntax error: expected filename after '<'\n"); return -1; }
            *infile = tok;
        } else if (strcmp(tok, ">") == 0) {
            tok = strtok(NULL, " \t\n");
            if (!tok) { fprintf(stderr, "syntax error: expected filename after '>'\n"); return -1; }
            *outfile = tok;
        } else {
            argv[argc++] = tok;
        }
        tok = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;
    return 0;
}

// execute a single command with optional infile/outfile redirection
static void execute_single(char **argv, char *infile, char *outfile) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        // child
        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) { perror("open infile"); exit(1); }
            if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2 infile"); exit(1); }
            close(fd);
        }
        if (outfile) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("open outfile"); exit(1); }
            if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2 outfile"); exit(1); }
            close(fd);
        }
        execvp(argv[0], argv);
        fprintf(stderr, "myshell: %s: %s\n", argv[0], strerror(errno));
        exit(127);
    } else {
        // parent
        int status;
        waitpid(pid, &status, 0);
    }
}

// execute a two-stage pipeline: left | right
// left_infile applies to left command (may be NULL), right_outfile applies to right command (may be NULL)
static void execute_pipe(char **left_argv, char *left_infile, char **right_argv, char *right_outfile) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork"); return; }
    if (pid1 == 0) {
        // left child: write end -> stdout
        close(pipefd[0]); // close read end
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) { perror("dup2 pipe write"); exit(1); }
        close(pipefd[1]);

        if (left_infile) {
            int fd = open(left_infile, O_RDONLY);
            if (fd < 0) { perror("open left infile"); exit(1); }
            if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2 left infile"); exit(1); }
            close(fd);
        }

        execvp(left_argv[0], left_argv);
        fprintf(stderr, "myshell: %s: %s\n", left_argv[0], strerror(errno));
        exit(127);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork"); return; }
    if (pid2 == 0) {
        // right child: read end -> stdin
        close(pipefd[1]); // close write end
        if (dup2(pipefd[0], STDIN_FILENO) < 0) { perror("dup2 pipe read"); exit(1); }
        close(pipefd[0]);

        if (right_outfile) {
            int fd = open(right_outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("open right outfile"); exit(1); }
            if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2 right outfile"); exit(1); }
            close(fd);
        }

        execvp(right_argv[0], right_argv);
        fprintf(stderr, "myshell: %s: %s\n", right_argv[0], strerror(errno));
        exit(127);
    }

    // parent
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

int main(void) {
    char *line = NULL;
    char *argv[MAXARGS];

    while (1) {
        line = readline(PROMPT);
        if (!line) { printf("\n"); break; }

        // skip leading whitespace
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#') { free(line); continue; }

        // handle !n re-exec
        if (p[0] == '!' && isdigit((unsigned char)p[1])) {
            int n = atoi(p + 1);
            char *cmd = get_history_command(n);
            if (!cmd) {
                fprintf(stderr, "No such history entry: %d\n", n);
                free(line);
                continue;
            }
            free(line);
            line = cmd;
            p = line;
            printf("Re-executing: %s\n", line);
        }

        // Support single pipe: check for '|' in the line
        char *pipe_pos = strchr(p, '|');
        if (pipe_pos) {
            // split into left and right (in-place)
            *pipe_pos = '\0';
            char *left = p;
            char *right = pipe_pos + 1;

            // trim leading spaces on right
            while (*right == ' ' || *right == '\t') right++;

            // parse left and right
            char *left_argv[MAXARGS];
            char *right_argv[MAXARGS];
            char *left_in = NULL, *left_out = NULL;
            char *right_in = NULL, *right_out = NULL;

            if (parse_simple_command(left, left_argv, &left_in, &left_out) < 0) {
                free(line);
                continue;
            }
            if (parse_simple_command(right, right_argv, &right_in, &right_out) < 0) {
                free(line);
                continue;
            }

            // Note: for a pipe, left_out is ignored (use pipe), right_in is ignored (stdin comes from pipe)
            // We will allow left_in (reading from file) and right_out (writing to file).
            execute_pipe(left_argv, left_in, right_argv, right_out);

            add_history_cmd(p);
            add_history(p);
            free(line);
            continue;
        }

        // No pipe: parse single command (may have < or >)
        char *infile = NULL;
        char *outfile = NULL;
        if (parse_simple_command(p, argv, &infile, &outfile) < 0) {
            free(line);
            continue;
        }

        // add to histories
        add_history_cmd(p);
        add_history(p);

        // handle builtins
        if (argv[0] == NULL) { free(line); continue; }
        if (handle_builtin(argv)) { free(line); continue; }

        // execute single command with possible redirection
        execute_single(argv, infile, outfile);

        free(line);
    }

    return 0;
}

