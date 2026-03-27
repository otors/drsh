#ifndef PARSER_H
#define PARSER_H
#include "types.h"

void parse_args(char *input, char *out[MAX_CMD_ARGS], redirect_t redirect[FD_AMOUNT]);
void parse_piped_cmds(char *input, char *out[MAX_PIPED_CMDS]);
#endif