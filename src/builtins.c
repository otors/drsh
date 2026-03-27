#include "builtins.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
#include <errno.h>
#include <readline/history.h>

sh_builtin_t sh_builtins[] = {
    {"exit", sh_exit},
    {"echo", sh_echo},
    {"type", sh_type},
    {"pwd", sh_pwd},
    {"cd", sh_cd},
    {"history", sh_history},
    {NULL, NULL},
};

sh_builtin_t sp_builtins[] = {
    {"exit", sh_exit},
    {"cd", sh_cd},
    {"history", sh_history},
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

int find_sh_builtins_by_prefix(const char *prefix, char **matches, int max)
{
    int count = 0;
    for (int i = 0; sh_builtins[i].name != NULL; i++)
    {
        if (count >= max)
            return count;
        if (strncmp(prefix, sh_builtins[i].name, strlen(prefix)) == 0)
        {
            matches[count++] = strdup(sh_builtins[i].name);
        }
    }
    return count;
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

int sh_pwd(char **args)
{
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
    return SH_CONTINUE;
}

int sh_cd(char **args)
{
    char *path = NULL;
    if (args[1] == NULL || strcmp(args[1], "~") == 0)
    {
        path = getenv("HOME");
    }
    if (path == NULL)
    {
        path = args[1];
    }
    if (chdir(path) != 0)
    {
        printf("%s: %s: %s\n", args[0], args[1], strerror(errno));
    }
    return SH_CONTINUE;
}

int sh_history(char **args)
{
    char history_path[1024];
    sprintf(history_path, "%s/.shell_history", getenv("HOME"));
    HIST_ENTRY **hist;
    int n = 0;

    if (args[1] != NULL && strcmp(args[1], "-r") == 0)
    {
        FILE *f = fopen(args[2], "r");
        if (!f)
        {
            perror("history_append_from_file");
            return SH_CONTINUE;
        }

        char line[4096];
        while (fgets(line, sizeof(line), f))
        {
            /* Strip trailing newline */
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n')
                line[len - 1] = '\0';

            if (line[0] != '\0') /* skip blank lines */
                add_history(line);
        }

        fclose(f);
        return SH_CONTINUE;
    }

    hist = history_list();
    if (hist == NULL)
        return SH_CONTINUE;

    while (hist[n] != NULL)
        n++;

    int start = 0;
    if (args[1] != NULL && strcmp(args[1], "-r") != 0)
        start = n - atoi(args[1]);
    if (start < 0)
        start = 0;

    for (int i = start; i < n; i++)
        printf("%d  %s\n", i + 1, hist[i]->line);

    return SH_CONTINUE;
}