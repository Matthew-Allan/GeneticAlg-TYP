#include "loader.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

Funcs load_imp(const char *file_path) {
    void *handle = dlopen(file_path, RTLD_LAZY);

    char *error = dlerror();
    if(error) {
        printf("%s\n", error);
        return NULL;
    }

    FuncLoader loader = dlsym(handle, "load_funcs");

    error = dlerror();
    if(error) {
        printf("%s\n", error);
        return NULL;
    }

    return loader();
} 