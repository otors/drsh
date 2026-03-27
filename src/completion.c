#include "path.h"
#include "builtins.h"
#include "completion.h"

#define MAX_MATCHES 64

char *sh_completer(const char *text, int state)
{
    static char *matches[MAX_MATCHES];
    static int match_idx = 0;
    static int match_count = 0;

    if (state == 0)
    {
        match_idx = 0;
        match_count = 0;
        match_count += find_sh_builtins_by_prefix(text, matches, MAX_MATCHES);
        match_count += search_path_by_prefix(text, matches, MAX_MATCHES, match_count);
    }

    if (match_idx >= match_count)
        return NULL;

    return matches[match_idx++];
}