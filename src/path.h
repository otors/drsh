#ifndef PATH_H
#define PATH_H
#ifdef _WIN32
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif

char *search_path(char *name);
#endif