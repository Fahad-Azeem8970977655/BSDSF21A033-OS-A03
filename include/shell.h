// shell.h - BSDSF21A033

#ifndef SHELL_H
#define SHELL_H

// Builtin function declarations
void print_help(void);
int handle_builtin(char **args);

// History feature (Feature 3)
#define HISTORY_SIZE 20
void add_history_cmd(const char *line);
void show_history(void);
char *get_history_command(int n);

#endif


