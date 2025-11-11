// shell.h - BSDSF21A033 (Features 1–7)

#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>  // for pid_t
#include <stddef.h>

// --- Built-in functions ---
void print_help(void);
int handle_builtin(char **args);

// --- History (Feature 3) ---
#define HISTORY_SIZE 20
void add_history_cmd(const char *line);
void show_history(void);
char *get_history_command(int n);

// --- Job control (Feature 6) ---
typedef struct job {
    int id;
    pid_t pid;
    char *cmdline;
    int running;
    struct job *next;
} job_t;

void add_job(pid_t pid, const char *cmdline);
void mark_job_done(pid_t pid);
void remove_job(pid_t pid);
void list_jobs(void);
int get_job_count(void);
void reap_jobs(void);

// --- Extended execution helpers (Feature 7) ---
int parse_simple_command(char *cmdline, char **argv, char **infile, char **outfile, int *append_out);
void execute_single(char **argv, char *infile, char *outfile, int append, int background);
void execute_pipeline(char **cmds, int nstages, int background);

#endif

