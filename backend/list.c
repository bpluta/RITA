#include "list.h"
#include <stdio.h>

void new_list(list *list, size_t element_size) {
    list->length = 0;
    list->element_size = element_size;
    list->head = NULL;
    list->tail = NULL;
}

void delete_list(list *list) {
    node *element = list->head;
    while (element != NULL) {
        if (list->deallocate_node) {
            list->deallocate_node(element->data);
        }
        node *next = element->next;
        free(element->data);
        free(element);
        element = next;
    }
}

void list_append(list *list, void *element) {
    node *node = malloc(sizeof(node));
    node->data = malloc(list->element_size);
    node->next = NULL;

    memcpy(node->data, element, list->element_size);

    if (list->length == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->length++;
}

void test_list() {
    list some_list;
    new_list(&some_list, sizeof(int));

    for (int i=1; i<=6; i++) {
        list_append(&some_list, &i);
    }

    node *element = some_list.head;
    for (int i=0; i<some_list.length; i++) {
        printf("element: %d\n", *(int*)element->data);
        element = element->next;
    }

    printf("============\n");

    element = some_list.head;
    while (element) {
        printf("element: %d\n", *(int*)element->data);
        element = element->next;
    }

    delete_list(&some_list);
}

typedef struct _some_struct {
    int *number;
} some_struct;

typedef struct _some_struct2 {
    char number;
} some_struct2;

void deallocator(some_struct *element) {
    free(element->number);
}

void test_dynamic_list() {
    list *some_list = malloc(sizeof(list));
    new_list(some_list, sizeof(some_struct));
    some_list->deallocate_node = (void*)(void*)deallocator;

    for (int i=1; i<=6; i++) {
        some_struct element;
        element.number = malloc(sizeof(int));
        memcpy(element.number, &i, sizeof(int));
        list_append(some_list, &element);
    }

    node *element = some_list->head;
    for (int i=0; i<some_list->length; i++) {
        some_struct *node = element->data;
        printf("element: %d\n", *node->number);
        element = element->next;
    }

    printf("============\n");

    element = some_list->head;
    while (element) {
        some_struct *node = element->data;
        printf("element: %d\n", *node->number);
        element = element->next;
    }

    delete_list(some_list);
}

void test_find(char *val) {
    list *some_list = malloc(sizeof(list));
    new_list(some_list, sizeof(some_struct2));

    for (char i=1; i<=6; i++) {
        some_struct2 element;
        element.number = i;
        list_append(some_list, &element);
    }

    node *element = some_list->head;
    while (element) {
        printf("iteration \n");
        some_struct2 *node = element->data;
        if (node->number == 4) {
            *val = node->number;
            printf("found result! == %d\n", node->number);

            break;
        }
        element = element->next;
    }

    delete_list(some_list);
}
