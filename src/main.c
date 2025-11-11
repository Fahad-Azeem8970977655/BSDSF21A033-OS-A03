// main.c - BSDSF21A033 (Features 1–7)
// Author: Fahad Azeem
// Implements base shell + builtins + history + readline + background jobs + multi-pipe + append redirection

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

#define MAXLINE 1024
#define MAXARGS 128
#define PROMPT "BSDSF21A033$ "

// ---------------------------------------------------------------------
// PARSER + EXECUTION HELPERS  (Feature 7)
// ---------------------------------------------------------------------

int parse_simple_command(char *cmdline, char **argv, char **infile, char **outfile, int *append_out) {
    int argc = 0;
    *infile = NULL;
    *outfile = NULL;
    *append_out = 0;

    // ✅ FIX: Strip newline before parsing (important for wc -l, grep, etc.)
    size_t len = strlen(cmdline);
    if (len > 0 && cmdline[len - 1] == '\n')
        cmdline[len - 1] = '\0';

    char *p = cmdline;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        // input redirection
        if (*p == '<') {
            p++;
            while (*p == ' ' || *p == '\t') p++;
            char *start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = '\0';
            *infile = start;
            continue;
        }

        // output redirection ( > or >> )
        if (*p == '>') {
            p++;
            if (*p == '>') { *append_out = 1; p++; } else *append_out = 0;
            while (*p == ' ' || *p == '\t') p++;
            char *start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = '\0';
            *outfile = start;
            continue;
        }

        // normal token
        argv[argc++] = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '<' && *p != '>') p++;
        if (*p) { *p++ = '\0'; }
        if (argc >= MAXARGS - 1) break;
    }

    argv[argc] = NULL;
    return argc;
}

static void execute_command_child(char **argv, char *infile, char *outfile, int append) {
    if (infile) {
        int fd = open(infile, O_RDONLY);
        if (fd < 0) { perror("open infile"); exit(1); }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (outfile) {
        int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
        int fd = open(outfile, flags, 0644);
        if (fd < 0) { perror("open outfile"); exit(1); }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    execvp(argv[0], argv);
    perror("execvp");
    exit(127);
}

void execute_single(char **argv, char *infile, char *outfile, int append, int background) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) {
        execute_command_child(argv, infile, outfile, append);
    } else {
        if (background) {
            add_job(pid, argv[0]);
            printf("[%d] %d running in background\n", get_job_count(), pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void execute_pipeline(char **cmds, int nstages, int background) {
    if (nstages <= 0) return;

    int prev_fd = -1;
    int pipefd[2];
    pid_t pids[MAXARGS];

    for (int i = 0; i < nstages; i++) {
        char *argv_stage[MAXARGS];
        char *infile = NULL, *outfile = NULL;
        int append = 0;
        int argc = parse_simple_command(cmds[i], argv_stage, &infile, &outfile, &append);
        if (argc <= 0) continue;

        if (i < nstages - 1 && pipe(pipefd) < 0) { perror("pipe"); return; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return; }

        if (pid == 0) {
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < nstages - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            char *use_in = (i == 0) ? infile : NULL;
            char *use_out = (i == nstages - 1) ? outfile : NULL;
            int use_append = (i == nstages - 1) ? append : 0;
            execute_command_child(argv_stage, use_in, use_out, use_append);
        } else {
            pids[i] = pid;
            if (prev_fd != -1) close(prev_fd);
            if (i < nstages - 1) {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
        }
    }

    if (background) {
        add_job(pids[0], cmds[0]);
        printf("[%d] %d pipeline started in background\n", get_job_count(), (int)pids[0]);
    } else {
        for (int i = 0; i < nstages; i++) waitpid(pids[i], NULL, 0);
    }
}

// ---------------------------------------------------------------------
// MAIN LOOP
// ---------------------------------------------------------------------

int main(void) {
    char *line;

    while (1) {
        reap_jobs();  // clean finished background jobs

        line = readline(PROMPT);
        if (!line) { printf("\n"); break; }

        // ✅ FIX: strip newline here too before processing
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (*line == '\0') { free(line); continue; }

        // skip comments
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\0') { free(line); continue; }

        // handle history recall !n
        if (p[0] == '!' && isdigit(p[1])) {
            int n = atoi(p + 1);
            char *cmd = get_history_command(n);
            if (!cmd) {
                fprintf(stderr, "No such history entry: %d\n", n);
                free(line);
                continue;
            }
            printf("Re-executing: %s\n", cmd);
            free(line);
            line = strdup(cmd);
            p = line;
        }

        add_history_cmd(p);

        // detect background
        int background = 0;
        len = strlen(p);
        if (len > 0 && p[len - 1] == '&') {
            background = 1;
            p[len - 1] = '\0';
        }

        // check built-ins
        char *temp = strdup(p);
        char *first_tok = strtok(temp, " \t");
        if (first_tok && handle_builtin(&first_tok)) {
            free(temp);
            free(line);
            continue;
        }
        free(temp);

        // split pipeline by '|'
        char *stages[64];
        int nstages = 0;
        char *token = strtok(p, "|");
        while (token && nstages < 64) {
            stages[nstages++] = token;
            token = strtok(NULL, "|");
        }

        if (nstages > 1)
            execute_pipeline(stages, nstages, background);
        else {
            char *argv[MAXARGS], *infile = NULL, *outfile = NULL;
            int append = 0;
            int argc = parse_simple_command(stages[0], argv, &infile, &outfile, &append);
            if (argc > 0)
                execute_single(argv, infile, outfile, append, background);
        }

        free(line);
    }
    return 0;
}

