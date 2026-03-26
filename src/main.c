#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
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
    printf("%s: command not found\n", input);
  }

  return 0;
}
