#ifndef TEST_H
#define TEST_H

#include "loader.h"

#include <stddef.h>

typedef void (* TestFunc)(void *imp, Funcs funcs);

typedef struct TestFuncs {
    void *(* create_imp)(Funcs funcs);
    void (* close_imp)(Funcs funcs, void *imp);
    void (* reset_imp)(Funcs funcs, void *imp);
    void (* display_imp)(Funcs funcs, void *imp);
    size_t (* size)(Funcs funcs, void *imp);
    size_t (* func_count)();
    const TestFunc *(* get_funcs)();
    const char **(* get_func_names)();
}* TestFuncs;

#endif