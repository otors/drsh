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
int find_sh_builtins_by_prefix(const char *prefix, char **matches, int max);
int sh_exit(char **args);
int sh_echo(char **args);
int sh_type(char **args);
int sh_pwd(char **args);
int sh_cd(char **args);
int sh_history(char **args);

extern sh_builtin_t sh_builtins[];
extern sh_builtin_t sp_builtins[];

#endif