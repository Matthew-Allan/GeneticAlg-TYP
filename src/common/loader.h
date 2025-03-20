#ifndef LIST_LOADER_H
#define LIST_LOADER_H

#define SHARED "./out/tests/"

typedef void * Funcs;

typedef Funcs (* FuncLoader)();

Funcs load_imp(const char *file_path);

#endif