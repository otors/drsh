#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdio.h>
#include <string.h>

void parse_args(char *input, char **out)
{
    char buf[1024];
    int buf_i = 0;
    char quotes = 0;
    int argc = 0;
    char escaped = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = input[i];
        if (escaped)
        {
            buf[buf_i++] = c;
            escaped = 0;
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
                    out[argc++] = strdup(buf);
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
        out[argc++] = strdup(buf);
        buf_i = 0;
    }
    out[argc] = NULL;
}