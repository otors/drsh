#include <dirent.h>
#include <string.h>
#include "../include/pathsep.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  const char *env_path = getenv("PATH");
  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char input[1024];
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = '\0';

    if (!strcmp(input, "exit"))
      break;

    if (!strncmp(input, "echo ", 5))
    {
      printf("%s\n", input + 5);
      continue;
    }

    if (!strncmp(input, "type ", 5))
    {
      char *arg = strdup(input + 5);
      char *arg_it, *dir_it;
      char *arg_tk = strtok_r(arg, " ", &arg_it);

      while (arg_tk != NULL)
      {
        if (!strcmp(arg_tk, "exit") || !strcmp(arg_tk, "echo") || !strcmp(arg_tk, "type"))
        {
          printf("%s is a shell builtin\n", arg_tk);
          arg_tk = strtok_r(NULL, " ", &arg_it);
          continue;
        }

        char *path_cp = strdup(env_path);
        char *dir_tk = strtok_r(path_cp, PATH_SEP, &dir_it);
        char found = 0;
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
            if (!strcmp(entry->d_name, arg_tk))
            {
              char full_path[1024];
              snprintf(full_path, sizeof(full_path), "%s/%s", dir_tk, entry->d_name);
              if (!access(full_path, X_OK))
              {
                printf("%s is %s\n", arg_tk, full_path);
                found = 1;
                break;
              }
            }
          }
          closedir(dir);
          if (found)
            break;
          dir_tk = strtok_r(NULL, PATH_SEP, &dir_it);
        }
        if (!found)
        {
          printf("%s: not found\n", arg_tk);
        }
        free(path_cp);
        arg_tk = strtok_r(NULL, " ", &arg_it);
      }
      free(arg);
      continue;
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
