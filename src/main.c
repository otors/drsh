#include <stdio.h>
#include <string.h>
#include "builtins.h"
#include <stdlib.h>
#include "path.h"
#include <unistd.h>
#include <sys/wait.h>
#include "parser.h"

int execute(char **args);

int main(int argc, char *argv[])
{
  char *args[64];
  int exit_code = 0;
  do
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char input[1024];
    fgets(input, sizeof(input), stdin);

    input[strlen(input) - 1] = '\0';

    parse_args(input, args);

    // char *arg_it;
    // char *arg_tk = strtok_r(input, " ", &arg_it);
    // char i = 0;
    // while (arg_tk != NULL)
    // {
    //   args[i++] = arg_tk;
    //   arg_tk = strtok_r(NULL, " ", &arg_it);
    // }
    // args[i] = NULL;
  } while (execute(args) != SH_EXIT);
  if (args[1] != NULL)
    exit_code = atoi(args[1]);
  return exit_code;
}

int execute(char **args)
{
  sh_builtin_t *builtin = find_sh_builtin(args[0]);
  if (builtin != NULL)
  {
    return builtin->func(args);
  }

  char *location = search_path(args[0]);
  if (location != NULL)
  {
    __pid_t pid = fork();
    if (pid == 0)
    {
      execve(location, args, __environ);
      perror("execve");
      exit(1);
    }
    else if (pid > 0)
    {
      int status;
      waitpid(pid, &status, 0);
      return SH_CONTINUE;
    }
    else
    {
      perror("fork");
    }
  }

  printf("%s: command not found\n", args[0]);
}