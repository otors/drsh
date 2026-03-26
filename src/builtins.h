#ifndef BUILTINS_H
#define BUILTINS_H
#include <stddef.h>
#define SH_CONTINUE 1
#define SH_EXIT 0

typedef struct
{
    char *name;
    int (*func)(char **args);
} sh_builtin_t;

sh_builtin_t *find_sh_builtin(char *name);
int sh_exit(char **args);
int sh_echo(char **args);
int sh_type(char **args);

extern sh_builtin_t sh_builtins[];

#endif
