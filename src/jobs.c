// jobs.c - BSDSF21A033
// Feature 6: Background Job Control

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static Job jobs[MAX_JOBS];
static int job_count = 0;

// Add a background job
void add_job(pid_t pid, const char *cmd) {
    if (job_count >= MAX_JOBS) {
        fprintf(stderr, "Job list full, cannot add more.\n");
        return;
    }
    jobs[job_count].id = job_count + 1;
    jobs[job_count].pid = pid;
    strncpy(jobs[job_count].cmd, cmd ? cmd : "", sizeof(jobs[job_count].cmd) - 1);
    jobs[job_count].running = 1;
    job_count++;
}

// Mark a job as done
void mark_job_done(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            jobs[i].running = 0;
            break;
        }
    }
}

// Remove a job completely (optional)
void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            break;
        }
    }
}

// List all background jobs
void jobs_list(void) {
    printf("ID\tPID\tStatus\t\tCommand\n");
    printf("-------------------------------------------\n");

    for (int i = 0; i < job_count; i++) {
        int status;
        pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);

        if (result == 0) {
            jobs[i].running = 1; // Still running
        } else {
            jobs[i].running = 0; // Completed
        }

        printf("[%d]\t%d\t%s\t%s\n",
               jobs[i].id,
               jobs[i].pid,
               jobs[i].running ? "Running" : "Done",
               jobs[i].cmd);
    }

    if (job_count == 0)
        printf("No background jobs.\n");
}

// Get number of background jobs
int get_job_count(void) {
    return job_count;
}

