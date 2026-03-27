#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "types.h"

void parse_args(char *input, char *out[MAX_CMD_ARGS], redirect_t redirect[FD_AMOUNT])
{
    char buf[1024];
    int buf_i = 0;
    char quotes = 0;
    int argc = 0;
    char escaped = 0;
    char do_redirect = 0;
    int fd = -1;
    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = input[i];
        if (escaped)
        {
            buf[buf_i++] = c;
            escaped = 0;
            continue;
        }
        if (c == '>')
        {
            if (do_redirect)
            {
                redirect[fd].append = 1;
                continue;
            }
            if (buf_i == 1 && (buf[0] == '1' || buf[0] == '2'))
            {
                fd = buf[0] - '0';
                buf_i = 0;
            }
            else
            {
                fd = 1;
                if (buf_i > 0)
                {
                    buf[buf_i] = '\0';
                    out[argc++] = strdup(buf);
                    buf_i = 0;
                }
            }
            do_redirect = 1;
            continue;
        }
        if (quotes == 0)
        {
            if (c == '\\')
            {
                escaped = 1;
                continue;
            }
            if (c == '\'')
            {
                quotes = 1;
            }
            else if (c == '\"')
            {
                quotes = 2;
            }
            else if (c == ' ')
            {
                if (buf_i > 0)
                {
                    buf[buf_i] = '\0';
                    if (do_redirect)
                    {
                        redirect[fd].file = strdup(buf);
                        do_redirect = 0;
                    }
                    else
                    {
                        out[argc++] = strdup(buf);
                    }
                    buf_i = 0;
                }
            }
            else
            {
                buf[buf_i++] = c;
            }
        }
        else if (quotes == 1)
        {
            if (c == '\'')
            {
                quotes = 0;
            }
            else
            {
                buf[buf_i++] = c;
            }
        }
        else if (quotes == 2)
        {
            if (c == '\\')
            {
                escaped = 1;
                continue;
            }
            if (c == '\"')
            {
                quotes = 0;
            }
            else
            {
                buf[buf_i++] = c;
            }
        }
    }
    if (buf_i > 0)
    {
        buf[buf_i] = '\0';
        if (do_redirect)
        {
            redirect[fd].file = strdup(buf);
            do_redirect = 0;
        }
        else
        {
            out[argc++] = strdup(buf);
        }
        buf_i = 0;
    }
    out[argc] = NULL;
}

void parse_piped_cmds(char *input, char *out[MAX_PIPED_CMDS])
{
    char *input_cp = strdup(input);
    char *pipe_it;
    char *pipe_tk = strtok_r(input_cp, "|", &pipe_it);
    int cmdc = 0;
    while (pipe_tk != NULL && cmdc < MAX_PIPES)
    {
        out[cmdc++] = strdup(pipe_tk);
        pipe_tk = strtok_r(NULL, "|", &pipe_it);
    }
    out[cmdc] = NULL;
}