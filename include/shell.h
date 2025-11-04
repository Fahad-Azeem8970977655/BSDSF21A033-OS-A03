#ifndef SHELL_H
#define SHELL_H

#define MAXLINE 1024
#define MAXARGS 128

// Built-in functions
int handle_builtin(char **args);
void print_help(void);

#endif // SHELL_H

