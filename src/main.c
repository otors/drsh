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

    char cmd[1024];
    fgets(cmd, sizeof(cmd), stdin);

    cmd[strcspn(cmd, "\n")] = '\0';

    printf("%s: command not found\n", cmd);
  }

  return 0;
}
