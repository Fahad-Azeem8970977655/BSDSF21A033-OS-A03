// history.c - BSDSF21A033 Feature 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

static char *history[HISTORY_SIZE];
static int history_count = 0;

void add_history_cmd(const char *line) {
    if (!line || *line == '\0') return;
    int idx = history_count % HISTORY_SIZE;
    free(history[idx]);
    history[idx] = strdup(line);
    history_count++;
}

void show_history(void) {
    int stored = history_count > HISTORY_SIZE ? HISTORY_SIZE : history_count;
    int start = history_count - stored;
    for (int i = 0; i < stored; i++) {
        int index = (start + i) % HISTORY_SIZE;
        printf("%d %s\n", i + 1, history[index]);
    }
}

char *get_history_command(int n) {
    int stored = history_count > HISTORY_SIZE ? HISTORY_SIZE : history_count;
    if (n < 1 || n > stored) return NULL;
    int start = history_count - stored;
    int index = (start + (n - 1)) % HISTORY_SIZE;
    return history[index] ? strdup(history[index]) : NULL;
}

