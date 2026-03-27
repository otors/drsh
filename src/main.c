#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "builtins.h"
#include "completion.h"
#include "parser.h"
#include "path.h"
#include "types.h"

int execute(char **args, redirect_t redirects[FD_AMOUNT]);
void apply_redirects(redirect_t redirects[FD_AMOUNT]);

int main(int argc, char *argv[])
{
  char *args[64];
  int status = SH_CONTINUE;
  while (status != SH_EXIT)
  {
    // Flush after every printf
    setbuf(stdout, NULL);
    rl_completion_entry_function = sh_completer;

    char *input = readline("$ ");
    if (input == NULL)
      break;

    redirect_t redirects[FD_AMOUNT];
    memset(&redirects, 0, sizeof(redirects));

    parse_args(input, args, redirects);
    status = execute(args, redirects);
    for (int i = 0; i < FD_AMOUNT; i++)
    {
      free(redirects[i].file);
    }
    free(input);
  };

  return args[1] != NULL ? atoi(args[1]) : 0;
}

int execute(char **args, redirect_t redirects[FD_AMOUNT])
{
  sh_builtin_t *builtin = find_sh_builtin(args[0]);
  if (builtin != NULL)
  {
    int saved[FD_AMOUNT];
    for (int i = 0; i < FD_AMOUNT; i++)
      saved[i] = dup(i);

    apply_redirects(redirects);
    int result = builtin->func(args);

    for (int i = 0; i < FD_AMOUNT; i++)
    {
      dup2(saved[i], i);
      close(saved[i]);
    }
    return result;
  }

  char *location = search_path(args[0]);
  if (location != NULL)
  {
    pid_t pid = fork();
    if (pid == 0)
    {
      apply_redirects(redirects);
      execve(location, args, __environ);
      free(location);
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
  return SH_CONTINUE;
}

void apply_redirects(redirect_t redirects[FD_AMOUNT])
{
  for (int i = 0; i < FD_AMOUNT; i++)
  {
    if (redirects[i].file == NULL)
      continue;
    int flags = i == 0 ? (O_RDONLY) : (O_WRONLY | O_CREAT | (redirects[i].append ? O_APPEND : O_TRUNC));
    int fd = open(redirects[i].file, flags, 0644);
    if (fd == -1)
    {
      perror("open");
      exit(1);
    }
    dup2(fd, i);
    close(fd);
  }
}