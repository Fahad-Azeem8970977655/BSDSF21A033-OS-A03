// main.c - BSDSF21A033
// Features 1–6: Base Shell, Builtins, History, Readline, I/O Redirection, Piping, Background Jobs

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXLINE 1024
#define MAXARGS 128
#define PROMPT "BSDSF21A033$ "

// ---------------------------------------------------------------
// I/O Redirection and Piping Helpers (Feature 5)
// ---------------------------------------------------------------

static int parse_simple_command(char *cmdline, char **argv, char **infile, char **outfile) {
    int argc = 0;
    *infile = NULL;
    *outfile = NULL;

    char *token = strtok(cmdline, " \t");
    while (token != NULL && argc < MAXARGS - 1) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token) *infile = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t");
            if (token) *outfile = token;
        } else {
            argv[argc++] = token;
        }
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    return argc;
}

static void execute_single(char **argv, char *infile, char *outfile, int background) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) { perror("input redirection"); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (outfile) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("output redirection"); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        if (background) {
            add_job(pid, argv[0]);   // Feature 6: track background job
            printf("[%d] %d running in background\n", get_job_count(), pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    } else {
        perror("fork");
    }
}

static void execute_pipe(char **left_argv, char *left_infile,
                         char **right_argv, char *right_outfile, int background) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        if (left_infile) {
            int in = open(left_infile, O_RDONLY);
            if (in < 0) { perror("open infile"); exit(1); }
            dup2(in, STDIN_FILENO);
            close(in);
        }
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(left_argv[0], left_argv);
        perror("execvp left");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        if (right_outfile) {
            int out = open(right_outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out < 0) { perror("open outfile"); exit(1); }
            dup2(out, STDOUT_FILENO);
            close(out);
        }
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(right_argv[0], right_argv);
        perror("execvp right");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    if (background) {
        add_job(pid1, left_argv[0]);
        add_job(pid2, right_argv[0]);
    } else {
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
}

// ---------------------------------------------------------------
// Main Loop (Features 1–6)
// ---------------------------------------------------------------

int main(void) {
    char *line;
    char *argv[MAXARGS];
    using_history();

    while (1) {
        line = readline(PROMPT);
        if (!line) {
            printf("\n");
            break;
        }

        // Trim leading spaces
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#') { free(line); continue; }

        // Handle !n history re-execution (Feature 3)
        if (p[0] == '!' && isdigit(p[1])) {
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

        // Add to both readline & custom history
        add_history(p);
        add_history_cmd(p);

        // Detect background job
        int background = 0;
        char *amp = strchr(p, '&');
        if (amp) {
            background = 1;
            *amp = '\0';
        }

        // Check for pipe
        char *pipe_pos = strchr(p, '|');
        if (pipe_pos) {
            *pipe_pos = '\0';
            char *left = p;
            char *right = pipe_pos + 1;

            char *left_argv[MAXARGS], *right_argv[MAXARGS];
            char *in1 = NULL, *out1 = NULL;
            char *in2 = NULL, *out2 = NULL;

            parse_simple_command(left, left_argv, &in1, &out1);
            parse_simple_command(right, right_argv, &in2, &out2);

            execute_pipe(left_argv, in1, right_argv, out2, background);
        } else {
            // Single command (with optional redirection)
            char *infile = NULL, *outfile = NULL;
            parse_simple_command(p, argv, &infile, &outfile);

            // Builtins first
            if (handle_builtin(argv)) {
                free(line);
                continue;
            }

            // Then external commands
            execute_single(argv, infile, outfile, background);
        }

        free(line);
    }
    return 0;
}

