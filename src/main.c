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
#include "search.h"
#include "types.h"

extern char **environ;

int execute(char *cmds[MAX_PIPED_CMDS], char *args[MAX_PIPED_CMDS][MAX_CMD_ARGS], redirect_t redirects[MAX_PIPED_CMDS][FD_AMOUNT]);
int execute_cmd(char **args, redirect_t redirects[FD_AMOUNT]);
void apply_redirects(redirect_t redirects[FD_AMOUNT]);
char **completion(const char *input, int start, int end);

int main(int argc, char *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  char *cmds[MAX_PIPED_CMDS];
  char *args[MAX_PIPED_CMDS][MAX_CMD_ARGS];
  redirect_t redirects[MAX_PIPED_CMDS][FD_AMOUNT];
  int status = SH_CONTINUE;

  // Flush after every printf
  setbuf(stdout, NULL);
  rl_attempted_completion_function = completion;

  char *history_path = getenv("HISTFILE");
  read_history(history_path);
  while (status != SH_EXIT)
  {
    char *input = readline("$ ");
    if (input == NULL)
      break;
    add_history(input);

    memset(&cmds, 0, sizeof(cmds));
    memset(&args, 0, sizeof(args));
    memset(&redirects, 0, sizeof(redirects));
    parse_piped_cmds(input, cmds);
    if (cmds[0] != NULL)
      status = execute(cmds, args, redirects);

    free(input);
  };
  write_history(history_path);
  return 0;
}

int execute(char *cmds[MAX_PIPED_CMDS], char *args[MAX_PIPED_CMDS][MAX_CMD_ARGS], redirect_t redirects[MAX_PIPED_CMDS][FD_AMOUNT])
{
  int pipefd[MAX_PIPES][2];
  pid_t pids[MAX_PIPED_CMDS];
  int n = 0;
  while (cmds[n] != NULL)
    n++;

  // parse all commands first
  for (int i = 0; i < n; i++)
    parse_args(cmds[i], args[i], redirects[i]);

  // handle special builtins only when no pipes
  if (n == 1 && args[0][0] != NULL)
  {
    for (int j = 0; sp_builtins[j].name != NULL; j++)
    {
      if (strcmp(args[0][0], sp_builtins[j].name) == 0)
      {
        int saved[FD_AMOUNT];
        for (int k = 0; k < FD_AMOUNT; k++)
          saved[k] = dup(k);
        apply_redirects(redirects[0]);
        int result = sp_builtins[j].func(args[0]);
        for (int k = 0; k < FD_AMOUNT; k++)
        {
          dup2(saved[k], k);
          close(saved[k]);
        }
        return result;
      }
    }
  }

  // create pipes
  for (int i = 0; i < n - 1; i++)
    pipe(pipefd[i]);

  // fork all commands
  for (int i = 0; i < n; i++)
  {
    pids[i] = fork();
    if (pids[i] == 0)
    {
      if (i < n - 1)
        dup2(pipefd[i][1], STDOUT_FILENO);
      if (i > 0)
        dup2(pipefd[i - 1][0], STDIN_FILENO);
      for (int p = 0; p < n - 1; p++)
      {
        close(pipefd[p][0]);
        close(pipefd[p][1]);
      }
      int status = execute_cmd(args[i], redirects[i]);
      for (int k = 0; k < FD_AMOUNT; k++)
        free(redirects[i][k].file);
      exit(status == SH_EXIT ? 42 : 0);
    }
  }

  // parent closes pipes
  for (int p = 0; p < n - 1; p++)
  {
    close(pipefd[p][0]);
    close(pipefd[p][1]);
  }

  // wait for all children
  int result = SH_CONTINUE;
  for (int i = 0; i < n; i++)
  {
    int status = 0;
    waitpid(pids[i], &status, 0);
    if (WEXITSTATUS(status) == 42)
      result = SH_EXIT;
  }
  return result;
}

int execute_cmd(char **args, redirect_t redirects[FD_AMOUNT])
{
  if (args[0] == NULL)
    return SH_CONTINUE;

  sh_builtin_t *builtin = find_sh_builtin(args[0]);
  if (builtin != NULL)
  {
    apply_redirects(redirects);
    return builtin->func(args);
  }

  char *location = search_path(args[0]);
  if (location != NULL)
  {
    apply_redirects(redirects);
    execve(location, args, environ);
    free(location);
    perror("execve");
    exit(1);
  }

  fprintf(stderr, "%s: command not found\n", args[0]);
  return SH_CONTINUE;
}

void apply_redirects(redirect_t redirects[FD_AMOUNT])
{
  for (int i = 0; i < FD_AMOUNT; i++)
  {
    if (redirects[i].file == NULL)
      continue;
    int flags = i == 0 ? O_RDONLY : O_WRONLY | O_CREAT | (redirects[i].append ? O_APPEND : O_TRUNC);
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

char **completion(const char *input, int start, int end)
{
  UNUSED(end);
  if (start == 0)
    return rl_completion_matches(input, sh_completer);
  rl_attempted_completion_over = 1;
  return rl_completion_matches(input, rl_filename_completion_function);
}