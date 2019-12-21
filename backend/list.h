#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node {
    void *data;
    struct _node *next;
} node;

typedef struct {
    int length;
    size_t element_size;
    node *head;
    node *tail;
    void (*deallocate_node)(void *);
} list;

void new_list(list *list, size_t element_size);
void delete_list(list *list);
void list_append(list *list, void *element);
void test_list();
void test_dynamic_list();
void test_find();

#endif
