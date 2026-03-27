#ifndef PATH_H
#define PATH_H
#ifdef _WIN32
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif

char *search_path(char *name);
int search_path_by_prefix(const char *prefix, char **matches, int max);
#endif