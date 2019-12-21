#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>

#define REGISTER_BUFFER_INITIAL_SIZE 2;

typedef struct _register_buffer register_buffer;

register_buffer register_buffer_init();
static void _register_buffer_clear(register_buffer *self);
static void _register_buffer_reinit(register_buffer *self);
static void _register_buffer_free(register_buffer *self);
static void _register_buffer_resize(register_buffer *self, int size);
static void _register_buffer_append(register_buffer *self, char reg);
static char* _register_buffer_get(register_buffer *self, int index);


typedef struct _register_buffer {
    char *data;
    int size;
    int count;

    void (*clear)(void *self);
    void (*reinit)(void *self);
    void (*free)(void *self);
    void (*resize)(void *self, int size);
    void (*append)(void *self, char reg);
    char *(*get)(void *self, int index);
} register_buffer;

#endif
