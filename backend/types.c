#include <stdlib.h>

#define REGISTER_BUFFER_INITIAL_SIZE 2;

typedef struct _registerBuffer RegisterBuffer;

RegisterBuffer registerBufferInit();
static void _registerBufferClear(RegisterBuffer *self);
static void _registerBufferReinit(RegisterBuffer *self);
static void _registerBufferResize(RegisterBuffer *self, int size);
static void _registerBufferAppend(RegisterBuffer *self, char reg);
static char* _registerBufferGet(RegisterBuffer *self, int index);


typedef struct _registerBuffer {
    char *data;
    int size;
    int count;

    void (*clear)(void *self);
    void (*reinit)(void *self);
    void (*resize)(void *self, int size);
    void (*append)(void *self, char reg);
    char *(*get)(void *self, int index);
} RegisterBuffer;

RegisterBuffer registerBufferInit() {
    RegisterBuffer buffer;

    buffer.size = REGISTER_BUFFER_INITIAL_SIZE;
    buffer.count = 0;
    buffer.data = calloc(1, sizeof(void *) * buffer.size);

    buffer.clear = (void *)&_registerBufferClear;
    buffer.reinit = (void *)&_registerBufferReinit;
    buffer.resize = (void *)&_registerBufferResize;
    buffer.append = (void *)&_registerBufferAppend;
    buffer.get = (void *)&_registerBufferGet;
    return buffer;
}

static void _registerBufferAppend(RegisterBuffer *self, char reg) {
    for (int i=0; i<self->count; i++) {
        if (self->data[i] == reg) return;
    }

    if (self->count == self->size) {
        _registerBufferResize(self, self->size * 2);
    }
    self->data[self->count++] = reg;
}

static void _registerBufferClear(RegisterBuffer *self) {
    for (int i=0; i<self->count; i++) {
        self->data[i] = (char)NULL;
    }
    self->count = 0;
}

static void _registerBufferReinit(RegisterBuffer *self) {
    free(self->data);
    *self = registerBufferInit();
}

static char* _registerBufferGet(RegisterBuffer *self, int index) {
    if (index < 0 || index >= self->count) {
        return NULL;
    }
    return &self->data[index];
}

static void _registerBufferResize(RegisterBuffer *self, int size) {
    self->data = realloc(self->data, sizeof(void *) * size);
    self->size = size;
}
