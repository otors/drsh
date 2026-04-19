#include "prompt.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *format_cwd(const char *cwd);
static const char *resolve_user(void);
static char *render_prompt(const char *user, const char *cwd, const char *symbol);

#define RL_ANSI(seq) "\001" seq "\002"
#define PROMPT_FMT RL_ANSI("\033[1;34m") "%s" RL_ANSI("\033[0m") " " \
                   RL_ANSI("\033[1;36m") "%s" RL_ANSI("\033[0m") " " \
                   RL_ANSI("\033[1;32m") "%s" RL_ANSI("\033[0m") " "

char *build_prompt(void)
{
  const char *user = resolve_user();
  char *cwd = getcwd(NULL, 0);
  if (cwd == NULL)
    cwd = strdup("?");
  char *pretty_cwd = format_cwd(cwd);
  const char *symbol = (geteuid() == 0) ? "#" : "$";
  char *prompt = render_prompt(user, pretty_cwd, symbol);

  free(cwd);
  free(pretty_cwd);
  return prompt;
}

static const char *resolve_user(void)
{
  const char *user = getenv("USER");
  if (user != NULL)
    return user;

  struct passwd *pw = getpwuid(getuid());
  if (pw != NULL && pw->pw_name != NULL)
    return pw->pw_name;

  return "user";
}

static char *render_prompt(const char *user, const char *cwd, const char *symbol)
{
  int needed = snprintf(NULL, 0, PROMPT_FMT, user, cwd, symbol);
  if (needed < 0)
    return strdup("$ ");

  char *prompt = malloc((size_t)needed + 1);
  if (prompt == NULL)
    return strdup("$ ");

  snprintf(prompt, (size_t)needed + 1, PROMPT_FMT, user, cwd, symbol);
  return prompt;
}

static char *format_cwd(const char *cwd)
{
  const char *home = getenv("HOME");
  if (home == NULL)
    return strdup(cwd);

  size_t home_len = strlen(home);
  if (strncmp(cwd, home, home_len) != 0)
    return strdup(cwd);

  if (cwd[home_len] != '\0' && cwd[home_len] != '/')
    return strdup(cwd);

  size_t suffix_len = strlen(cwd + home_len);
  char *result = malloc(1 + suffix_len + 1);
  if (result == NULL)
    return strdup(cwd);

  result[0] = '~';
  memcpy(result + 1, cwd + home_len, suffix_len + 1);
  return result;
}
