#ifndef TYPES_H
#define TYPES_H

#define FD_AMOUNT 3
#define MAX_PIPES 16
#define MAX_PIPED_CMDS MAX_PIPES + 1
#define MAX_CMD_ARGS 64

typedef struct
{
    char *file;
    int append;
} redirect_t;
#endif
