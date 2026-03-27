#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "path.h"

char *search_path(char *name)
{
    char *env_path = getenv("PATH");
    if (env_path == NULL)
        return NULL;
    char *path_cp = strdup(env_path);
    char *dir_it;
    char *dir_tk = strtok_r(path_cp, PATH_SEP, &dir_it);
    while (dir_tk != NULL)
    {
        DIR *dir;
        if ((dir = opendir(dir_tk)) == NULL)
        {
            dir_tk = strtok_r(NULL, PATH_SEP, &dir_it);
            continue;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (!strcmp(entry->d_name, name))
            {
                char *full_path = malloc(strlen(dir_tk) + strlen(name) + 2);
                sprintf(full_path, "%s/%s\0", dir_tk, entry->d_name);
                if (!access(full_path, X_OK))
                {

                    closedir(dir);
                    free(path_cp);
                    return full_path;
                }
                free(full_path);
            }
        }
        closedir(dir);
        dir_tk = strtok_r(NULL, PATH_SEP, &dir_it);
    }
    free(path_cp);
    return NULL;
}

int search_path_by_prefix(const char *prefix, char **matches, int max, int already_found)
{
    char **to_add = matches + already_found;
    int count = already_found;
    char *env_path = getenv("PATH");
    if (env_path == NULL)
        return 0;
    char *path_cp = strdup(env_path);
    char *dir_it;
    char *dir_tk = strtok_r(path_cp, PATH_SEP, &dir_it);
    while (dir_tk != NULL && count < max)
    {
        DIR *dir;
        if ((dir = opendir(dir_tk)) == NULL)
        {
            dir_tk = strtok_r(NULL, PATH_SEP, &dir_it);
            continue;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (!strncmp(entry->d_name, prefix, strlen(prefix)))
            {
                char *full_path = malloc(strlen(dir_tk) + strlen(entry->d_name) + 2);
                sprintf(full_path, "%s/%s", dir_tk, entry->d_name);
                if (!access(full_path, X_OK))
                {
                    to_add[count++] = strdup(entry->d_name);
                    continue;
                }
                free(full_path);
            }
        }
        closedir(dir);
        dir_tk = strtok_r(NULL, PATH_SEP, &dir_it);
    }
    free(path_cp);
    return count;
}