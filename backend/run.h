#ifndef RUN_H
#define RUN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <mach/mach.h>
#include <sys/ptrace.h>
#include <assert.h>
#include <mach/mach_types.h>
#include <mach-o/dyld_images.h>
#include <stdbool.h>
#include <time.h>
#include "msg.h"
#include "session.h"
#include "process.h"
#include "dyld_data.h"

#define _POSIX_SPAWN_DISABLE_ASLR 0x0100

#define BREAKPOINT_OPCODE 0x33

#define HW_BREAKPOINT_TYPE_X 0
#define HW_BREAKPOINT_TYPE_W 1
#define HW_BREAKPOINT_TYPE_RW 3

void handle_error(kern_return_t err, char *type);
void get_threads(mach_port_t *task, thread_act_array_t *threads);
void print_state(x86_thread_state_t *state);
void get_state(thread_act_t *thread, x86_thread_state_t *state);
void set_state(thread_act_t *thread, x86_thread_state_t *state);
void step(mach_port_t *task, thread_act_t *thread, x86_thread_state_t *state);
void attach(pid_t pid);
void detach(pid_t pid);

bool continue_execution(mach_port_t *task, thread_act_t *thread, x86_thread_state_t *state);
bool continue_until_breakpoint(debug_session *session);
bool set_breakpoint(debug_session *session, uint64_t address);
bool set_hardware_watchpoint(debug_session *session, uint64_t address, int num, int type, size_t len);
bool clear_hardware_watchpoint(debug_session *session);
bool restore_breakpoint_instruction(debug_session *session, uint64_t address);
bool find_breakpoint(list *breakpoints, uint64_t address, char *result);
bool is_overable(debug_session *session, uint64_t address);
void debug(debug_session *session, void (*callback)(debug_session*));
pid_t spawn_process(char *file_path);

#endif
