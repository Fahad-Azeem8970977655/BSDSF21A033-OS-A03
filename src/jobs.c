// jobs.c - BSDSF21A033 (Feature 6 + 7)
// Background job management implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"

#define MAX_JOBS 100

static job_t *job_head = NULL;
static int next_job_id = 1;

// ------------------------------------------------------------
// Add new background job
// ------------------------------------------------------------
void add_job(pid_t pid, const char *cmdline) {
    job_t *j = malloc(sizeof(job_t));
    if (!j) return;
    j->id = next_job_id++;
    j->pid = pid;
    j->cmdline = strdup(cmdline ? cmdline : "");
    j->running = 1;
    j->next = job_head;
    job_head = j;

    printf("[%d] %d started in background\n", j->id, j->pid);
}

// ------------------------------------------------------------
// Mark job as done (called by reap_jobs())
// ------------------------------------------------------------
void mark_job_done(pid_t pid) {
    for (job_t *p = job_head; p; p = p->next) {
        if (p->pid == pid) {
            p->running = 0;
            break;
        }
    }
}

// ------------------------------------------------------------
// Remove job from list
// ------------------------------------------------------------
void remove_job(pid_t pid) {
    job_t **pp = &job_head;
    while (*pp) {
        job_t *cur = *pp;
        if (cur->pid == pid) {
            *pp = cur->next;
            free(cur->cmdline);
            free(cur);
            return;
        }
        pp = &cur->next;
    }
}

// ------------------------------------------------------------
// List all background jobs
// ------------------------------------------------------------
void list_jobs(void) {
    printf("ID\tPID\tStatus\t\tCommand\n");
    printf("-------------------------------------------\n");
    for (job_t *p = job_head; p; p = p->next) {
        printf("[%d]\t%d\t%s\t%s\n",
               p->id,
               p->pid,
               p->running ? "Running" : "Done",
               p->cmdline);
    }
}

// ------------------------------------------------------------
// Return total number of jobs
// ------------------------------------------------------------
int get_job_count(void) {
    int count = 0;
    for (job_t *p = job_head; p; p = p->next) count++;
    return count;
}

// ------------------------------------------------------------
// Reap finished background jobs
// ------------------------------------------------------------
void reap_jobs(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        mark_job_done(pid);
        remove_job(pid);
    }
}

