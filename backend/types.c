#include "types.h"

register_buffer register_buffer_init() {
    register_buffer buffer;

    buffer.size = REGISTER_BUFFER_INITIAL_SIZE;
    buffer.count = 0;
    buffer.data = calloc(1, sizeof(void *) * buffer.size);

    buffer.clear = (void *)&_register_buffer_clear;
    buffer.reinit = (void *)&_register_buffer_reinit;
    buffer.resize = (void *)&_register_buffer_resize;
    buffer.append = (void *)&_register_buffer_append;
    buffer.free = (void *)&_register_buffer_free;
    buffer.get = (void *)&_register_buffer_get;
    return buffer;
}

static void _register_buffer_append(register_buffer *self, char reg) {
    for (int i=0; i<self->count; i++) {
        if (self->data[i] == reg) return;
    }

    if (self->count == self->size) {
        _register_buffer_resize(self, self->size * 2);
    }
    self->data[self->count++] = reg;
}

static void _register_buffer_clear(register_buffer *self) {
    for (int i=0; i<self->count; i++) {
        self->data[i] = (char)NULL;
    }
    self->count = 0;
}

static void _register_buffer_free(register_buffer *self) {
    free(self->data);
}

static void _register_buffer_reinit(register_buffer *self) {
    free(self->data);
    *self = register_buffer_init();
}

static char* _register_buffer_get(register_buffer *self, int index) {
    if (index < 0 || index >= self->count) {
        return NULL;
    }
    return &self->data[index];
}

static void _register_buffer_resize(register_buffer *self, int size) {
    self->data = realloc(self->data, sizeof(void *) * size);
    self->size = size;
}
