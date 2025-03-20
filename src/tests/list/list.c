#include "testfuncs.h"
#include "listfuncs.h"

#include <stdlib.h>
#include <stdio.h>

#define list(funcs) ((ListFuncs)(funcs))

static int index_exc_(void *imp, ListFuncs funcs) {
    return funcs->size(imp) ? rand() % funcs->size(imp) : -1;
}

static int index_inc_(void *imp, ListFuncs funcs) {
    return funcs->size(imp) ? rand() % (funcs->size(imp) + 1) : 0;
}

static void add_(void *imp, Funcs funcs) {
    list(funcs)->add_int(imp, rand());
}

static void insert_(void *imp, Funcs funcs) {
    list(funcs)->add_int_at(imp, rand(), index_exc_(imp, funcs));
}

static void set_(void *imp, Funcs funcs) {
    list(funcs)->set_int(imp, rand(), index_exc_(imp, funcs));
}

static void get_(void *imp, Funcs funcs) {
    list(funcs)->get_int(imp, index_exc_(imp, funcs));
}

static void remove_(void *imp, Funcs funcs) {
    list(funcs)->remove_int(imp, index_exc_(imp, funcs));
}

static void pop_(void *imp, Funcs funcs) {
    list(funcs)->remove_int(imp, list(funcs)->size(imp) - 1);
}

static void contains_(void *imp, Funcs funcs) {
    list(funcs)->contains_int(imp, rand());
}

static void brad_(void *imp, Funcs funcs) {
    int things[12] = {69, 420, 0, 0, 0, 42, 21, 9, 10, 2004, -2003, 1776};
    list(funcs)->add_many_at(imp, things, index_inc_(imp, funcs), 12);
}

static const TestFunc funcs[] = {
    add_, insert_, set_, get_, remove_, pop_, contains_
};

static const char *func_names[] = {
    "add_", "insert_", "set_", "get_", "remove_", "pop_", "contains_", "brad_"
};

static const TestFunc *get_funcs_() {
    return funcs;
}

static const char **get_func_names_() {
    return func_names;
}

static size_t func_count_() {
    return sizeof(funcs) / sizeof(TestFunc);
}

static void *create_imp_(Funcs funcs) {
    return list(funcs)->create_list(sizeof(int));
}

static void close_imp_(Funcs funcs, void *imp) {
    list(funcs)->destroy_list(imp);
}

static void reset_imp_(Funcs funcs, void *imp) {
    list(funcs)->clear_list(imp);
}

static void display_imp_(Funcs funcs, void *imp) {
    list(funcs)->print_list(imp);
}

static size_t size_(Funcs funcs, void *imp) {
    return list(funcs)->size(imp);
}

TestFuncs load_funcs() {
    TestFuncs funcs = (TestFuncs) malloc(sizeof(struct TestFuncs));
    funcs->get_funcs = get_funcs_;
    funcs->get_func_names = get_func_names_;
    funcs->func_count = func_count_;
    funcs->create_imp = create_imp_;
    funcs->close_imp = close_imp_;
    funcs->reset_imp = reset_imp_;
    funcs->display_imp = display_imp_;
    funcs->size = size_;
    return funcs;
}