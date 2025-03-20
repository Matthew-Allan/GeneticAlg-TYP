#include "listfuncs.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct LinkedListNode {
    struct LinkedListNode *next;
    struct LinkedListNode *prev;
}* LinkedListNode;

typedef struct LinkedList {
    LinkedListNode head;
    LinkedListNode tail;
    size_t size;
    size_t item_size;
}* LinkedList;

#define lnk(list) ((LinkedList)(list))

#define node_data(node) ((void *)((LinkedListNode)(node) + 1))

#define item_ind(list, item, index) (((char *) (item))[lnk(list)->item_size * (index)])

static size_t size_(List list) {
    return lnk(list)->size;
}

static LinkedListNode create_node_(LinkedList list, void *item) {
    LinkedListNode node = (LinkedListNode) malloc(sizeof(struct LinkedListNode) + list->item_size);
    node->next = NULL;
    node->prev = NULL;
    memcpy(node_data(node), item, list->item_size);
    return node;
}

static List create_list_(size_t item_size) {
    LinkedList list = (LinkedList) malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->item_size = item_size;
    return list;
}

static void clear_list_(List list) {
    LinkedListNode next;
    for(LinkedListNode item = lnk(list)->head; item; item = next) {
        next = item->next;
        free(item);
    }
    lnk(list)->head = NULL;
    lnk(list)->tail = NULL;
    lnk(list)->size = 0;
    return;
}

static void destroy_list_(List list) {
    clear_list_(list);
    free(list);
}

static int check_index_inc_(LinkedList list, off_t index) {
    return index >= 0 && index <= list->size;
}

static int check_index_exc_(LinkedList list, off_t index) {
    return index >= 0 && index < list->size;
}

static LinkedListNode get_node_b_(LinkedList list, off_t index) {
    LinkedListNode node = list->tail;
    for(off_t i = list->size - 1; i > index; i--) {
        node = node->prev;
    } 
    return node;
}

static LinkedListNode get_node_f_(LinkedList list, off_t index) {
    LinkedListNode node = list->head;
    for(off_t i = 0; i < index; i++) {
        node = node->next;
    } 
    return node;
}

static LinkedListNode get_node_(LinkedList list, off_t index) {
    if(!check_index_inc_(list, index)) {
        return NULL;
    }
    return (index < list->size / 2 ? get_node_f_ : get_node_b_)(list, index);
}

static void add_many_at_(List list, void *item, off_t index, size_t count) {
    if(!check_index_inc_(list, index) || count <= 0) {
        return;
    }
    LinkedListNode start_node = create_node_(list, item);
    LinkedListNode final_node = start_node;

    for(int i = 1; i < count; i++) {
        LinkedListNode new_node = create_node_(list, &item_ind(list, item, i));
        final_node->next = new_node;
        new_node->prev = final_node;
        final_node = new_node;
    }
    LinkedList lista = lnk(list);

    LinkedListNode prev = get_node_(list, index - 1);
    LinkedListNode next = prev ? prev->next : get_node_(list, index);

    if(index == 0) {
        final_node->next = lnk(list)->head;
        lnk(list)->head = start_node;
    } else {
        final_node->next = prev->next;
        prev->next = start_node;
    }

    if(index == lnk(list)->size) {
        start_node->prev = lnk(list)->tail;
        lnk(list)->tail = final_node;
    } else {
        start_node->prev = next->prev;
        next->prev = final_node;
    }

    lnk(list)->size += count;
}

static void add_many_(List list, void *item, size_t count) {
    add_many_at_(list, item, lnk(list)->size, count);
}

static void add_at_(List list, void *item, off_t index) {
    add_many_at_(list, item, index, 1);
}

static void add_(List list, void *item) {
    add_many_at_(list, item, lnk(list)->size, 1);
}

static void get_node_data_(LinkedList list, LinkedListNode node, void *out) {
    memcpy(out, node_data(node), lnk(list)->item_size);
}

static void get_(List list, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    get_node_data_(list, get_node_(list, index), out);
}

static void set_(List list, void *item, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    LinkedListNode node = get_node_(list, index);
    get_node_data_(list, node, out);
    memcpy(node_data(node), item, lnk(list)->item_size);
}

static void remove_at_(List list, off_t index, void *out) {
    if(!check_index_exc_(list, index)) {
        return;
    }
    LinkedListNode node = get_node_(list, index);
    get_node_data_(list, node, out);

    if(node->prev) {
        node->prev->next = node->next;
    } else {
        lnk(list)->head = node->next;
    }

    if(node->next) {
        node->next->prev = node->prev;
    } else {
        lnk(list)->tail = node->prev;
    }

    lnk(list)->size--;

    free(node);
}

static ssize_t index_of_(List list, void *item) {
    LinkedListNode node = lnk(list)->head;
    for(off_t i = 0; node; node = node->next, i++) {
        if(memcmp(item, node_data(node), lnk(list)->item_size) == 0) {
            return i;
        }
    }
    return -1;
}

static int contains_(List list, void *item) {
    return index_of_(list, item) >= 0;
}

static void add_int_(List list, int item) {
    add_(list, &item);
}

static void add_int_at_(List list, int item, off_t index) {
    add_at_(list, &item, index);
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
    int val;
    remove_at_(list, index, &val);
    return val;
}

static ssize_t index_of_int_(List list, int item) {
    index_of_(list, &item);
}

static int contains_int_(List list, int item) {
    contains_(list, &item);
}

static void print_list_(List list) {
    printf("{");
    for(int i = 0; i < lnk(list)->size; i++) {
        printf("%d", get_int_(list, i));
        if(i < lnk(list)->size - 1) {
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