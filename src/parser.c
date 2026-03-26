#include "parser.h"
#include <stdio.h>
#include <string.h>

void parse_args(char *input, char *out[])
{
    char in_quotes = 0;
    int begin = 0;
    int argc = 0;
    int i = 0;
    while (input[i] != '\0')
    {
        if (input[i] == '\'')
        {
            if (!in_quotes)
            {
                begin = i + 1;
                in_quotes = 1;
            }
            else
            {
                input[i] = '\0';
                out[argc++] = input + begin;
                begin = i + 1;
                in_quotes = 0;
            }
        }
        else if (!in_quotes && input[i] == ' ')
        {
            if (i > begin && argc < 63)
            {
                input[i] = '\0';
                out[argc++] = input + begin;
            }
            begin = i + 1;
        }
        i++;
    }
    if (i > begin && argc < 63)
        out[argc++] = input + begin;
    out[argc] = NULL;
}