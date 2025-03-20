#include "listfuncs.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stddef.h>
#include <sys/types.h>

#define ARR_LIST_DEF_LEN 10

typedef struct ArrayList {
    void *array;
    size_t size;
    size_t array_size;
    size_t item_size;
}* ArrayList;

#define arr(list) ((ArrayList)(list))

static size_t size_(List list) {
    return arr(list)->size;
}

static int resize_array_(ArrayList list) {
    ArrayList arr_list = arr(list);
    void *old_array = arr_list->array;
    
    if((list->array = (void *) malloc(list->array_size * list->item_size)) == NULL) {
        list->array = old_array;
        return -1;
    }

    if(list->array == NULL) {
        return 0;
    }
    
    memcpy(list->array, old_array, list->size * list->item_size);
    free(old_array);
    
    return 0;
}

static int grow_(ArrayList list, size_t min_growth) {
    size_t old_size = list->array_size;
    while(list->array_size - old_size < min_growth) {
        list->array_size += list->array_size >> 1;
    }
    if(resize_array_(list) == 0) {
        return 0;
    }
    list->array_size = old_size + min_growth;
    return resize_array_(list);
}

static int check_index_inc_(ArrayList list, off_t index) {
    return index >= 0 && index <= list->size;
}

static int check_index_exc_(ArrayList list, off_t index) {
    return index >= 0 && index < list->size;
}

static void add_base_(ArrayList list, void *items, off_t index, size_t count) {
    if(!check_index_inc_(list, index) || count <= 0) {
        return;
    }
    if(list->size + count > list->array_size) {
        grow_(list, count);
    }
    memmove(list->array + (index + count) * list->item_size, list->array + index * list->item_size, (list->size - index) * list->item_size);
    memcpy(list->array + index * list->item_size, items, list->item_size * count);
    list->size += count;
}

static void add_(List list, void *item) {
    add_base_(list, item, arr(list)->size, 1);
}

static void add_at_(List list, void *item, off_t index) {
    add_base_(list, item, index, 1);
}

static void add_many_(List list, void *item, size_t count) {
    add_base_(list, item, arr(list)->size, count);
}

static void add_many_at_(List list, void *item, off_t index, size_t count) {
    add_base_(list, item, index, count);
}

static void get_(List list, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    memcpy(out, arr(list)->array + index * arr(list)->item_size, arr(list)->item_size);
}

static void set_(List list, void *item, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    get_(list, index, out);
    memcpy(arr(list)->array + index * arr(list)->item_size, item, arr(list)->item_size);
}

static void remove_at_(List list, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    if(out != NULL) {
        get_(list, index, out);
    }
    memmove(arr(list)->array + index * arr(list)->item_size, arr(list)->array + (index + 1) * arr(list)->item_size, (arr(list)->size - index) * arr(list)->item_size);
    arr(list)->size--;
}

static ssize_t index_of_(List list, void *item) {
    for(int i = 0; i < arr(list)->size; i++) {
        if(memcmp(arr(list)->array + i * arr(list)->item_size, item, arr(list)->item_size) == 0)
            return i;
    }
    return -1;
}

static int contains_(List list, void *item) {
    return index_of_(list, item) >= 0;
}

static void add_int_(List list, int item) {
    add_base_(list, &item, arr(list)->size, 1);
}

static void add_int_at_(List list, int item, off_t index) {
    add_base_(list, &item, index, 1);
}

static int get_int_(List list, off_t index) {
    int item;
    get_(list, index, &item);
    return item;
}

static int set_int_(List list, int item, off_t index) {
    int old;
    set_(list, &item, index, &old);
    return old;
}

static int remove_int_(List list, off_t index) {
    int item;
    remove_at_(list, index, &item);
    return item;
}

static ssize_t index_of_int_(List list, int item) {
    for(int i = 0; i < arr(list)->size; i++) {
        if(((int *)arr(list)->array)[i] == item)
            return i;
    }
    return -1;
}

static int contains_int_(List list, int item) {
    return index_of_int_(list, item) >= 0;
}

static List create_list_(size_t item_size) {
    if(item_size == 0) {
        return NULL;
    }
    ArrayList list = (ArrayList) malloc(sizeof(struct ArrayList));

    if(list == NULL) {
        return NULL;
    }

    list->size = 0;
    list->array_size = ARR_LIST_DEF_LEN;
    list->item_size = item_size;
    list->array = NULL;

    if(resize_array_(list) < 0) {
        free(list);
        list = NULL;
    }

    return list;
}

static void clear_list_(List list) {
    free(arr(list)->array);
    arr(list)->array = NULL;
    arr(list)->array_size = ARR_LIST_DEF_LEN;
    arr(list)->size = 0;
    resize_array_(list);
}

static void destroy_list_(List list) {
    free(arr(list)->array);
    free(list);
}

static void print_list_(List list) {
    printf("{");
    for(int i = 0; i < arr(list)->size; i++) {
        printf("%d", get_int_(list, i));
        if(i < arr(list)->size - 1) {
            printf(", ");
        }
    }
    printf("}\n");
}

ListFuncs load_funcs() {
    ListFuncs funcs = (ListFuncs) malloc(sizeof(struct ListFuncs));
    funcs->create_list = create_list_;
    funcs->destroy_list = destroy_list_;
    funcs->clear_list = clear_list_;
    funcs->size = size_;
    funcs->add = add_;
    funcs->add_at = add_at_;
    funcs->add_many = add_many_;
    funcs->add_many_at = add_many_at_;
    funcs->set = set_;
    funcs->get = get_;
    funcs->remove_at = remove_at_;
    funcs->index_of = index_of_;
    funcs->contains = contains_;
    funcs->add_int = add_int_;
    funcs->add_int_at = add_int_at_;
    funcs->get_int = get_int_;
    funcs->set_int = set_int_;
    funcs->remove_int = remove_int_;
    funcs->index_of_int = index_of_int_;
    funcs->contains_int = contains_int_;
    funcs->print_list = print_list_;
    return funcs;
}