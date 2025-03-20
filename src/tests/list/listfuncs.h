#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <sys/types.h>

typedef void *List;

typedef struct ListFuncs {
    List (* create_list)(size_t item_size);
    void (* destroy_list)(List list);
    void (* clear_list)(List list);
    size_t (* size)(List list);
    
    void (* add)(List list, void *item);
    void (* add_at)(List list, void *item, off_t index);
    void (* add_many)(List list, void *item, size_t count);
    void (* add_many_at)(List list, void *item, off_t index, size_t count);
    void (* set)(List list, void *item, off_t index, void *out);
    void (* get)(List list, off_t index, void *out);
    void (* remove_at)(List list, off_t index, void *out);
    ssize_t (* index_of)(List list, void *item);
    int (* contains)(List list, void *item);
    
    void (* add_int)(List list, int item);
    void (* add_int_at)(List list, int item, off_t index);
    int (* get_int)(List list, off_t index);
    int (* set_int)(List list, int item, off_t index);
    int (* remove_int)(List list, off_t index);
    ssize_t (* index_of_int)(List list, int item);
    int (* contains_int)(List list, int item);
    
    void (* print_list)(List list);
}* ListFuncs;

#endif