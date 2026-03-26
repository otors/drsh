#include "builtins.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"

sh_builtin_t sh_builtins[] = {
    {"exit", sh_exit},
    {"echo", sh_echo},
    {"type", sh_type},
    {NULL, NULL},
};

sh_builtin_t *find_sh_builtin(char *name)
{
    for (int i = 0; sh_builtins[i].name != NULL; i++)
    {
        if (strcmp(name, sh_builtins[i].name) == 0)
        {
            return &sh_builtins[i];
        }
    }
    return NULL;
}

int sh_exit(char **args)
{
    return SH_EXIT;
}

int sh_echo(char **args)
{
    for (int i = 1; args[i] != NULL; i++)
    {
        printf("%s", args[i]);
        if (args[i + 1] != NULL)
            printf(" ");
    }
    printf("\n");
    return SH_CONTINUE;
}

int sh_type(char **args)
{
    for (int i = 1; args[i] != NULL; i++)
    {
        sh_builtin_t *builtin = find_sh_builtin(args[i]);
        if (builtin != NULL)
        {
            printf("%s is a shell builtin\n", args[i]);
            continue;
        }

        char *location = search_path(args[i]);
        if (location != NULL)
        {
            printf("%s is %s\n", args[i], location);
            continue;
        }

        printf("%s: not found\n", args[i]);
    }
    return SH_CONTINUE;
}
