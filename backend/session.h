#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <mach/mach.h>
#include "list.h"
#include "types.h"

typedef struct _debug_session debug_session;
typedef struct _breakpoint breakpoint;

#include "dyld_data.h"

struct _debug_session {
    char *path;
    pid_t pid;
    mach_port_t task;
    thread_act_t thread;
    uint64_t entry_point;
    uint64_t instruction_count;
    x86_thread_state_t current_state;
    register_buffer recently_modified_registers;
    list recently_modified_memory;

    list breakpoints;
    list executable_libs;
};

struct _breakpoint {
    uint64_t address;
    char saved_data;
};

void init_debug_session(debug_session *session);
void delete_debug_session(debug_session *session);

#endif
