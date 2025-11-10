// shell.h - BSDSF21A033
// Header file for custom shell (Features 1–6)

#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>   // ✅ for pid_t
#include <stddef.h>

// -----------------------------
// Built-in Command Prototypes
// -----------------------------
void print_help(void);
int handle_builtin(char **args);

// -----------------------------
// Command History (Feature 3)
// -----------------------------
#define HISTORY_SIZE 20
void add_history_cmd(const char *line);
void show_history(void);
char *get_history_command(int n);

// -----------------------------
// Background Jobs (Feature 6)
// -----------------------------
#define MAX_JOBS 50

typedef struct {
    int id;              // Job ID
    pid_t pid;           // Process ID
    char cmd[256];       // Command line
    int running;         // 1 = running, 0 = done
} Job;

// Job control functions
void add_job(pid_t pid, const char *cmd);
void mark_job_done(pid_t pid);
void remove_job(pid_t pid);
void jobs_list(void);
int get_job_count(void);

#endif

