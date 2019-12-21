#include "run.h"

int instruction_counter = 0;

void debug(debug_session *session, void (*callback)(debug_session*)) { // make callback here
    thread_act_array_t threads = NULL;
    get_threads(&session->task, &threads);
    thread_act_t current_thread = (thread_act_t)threads[0];
    session->thread = current_thread;

    x86_thread_state_t state;
    get_state(&session->thread, &state);

    unsigned long long current_pointer = (unsigned long long)state.uts.ts64.__rip;
    printf("Start position : %#016llx\n", current_pointer);

    printf("%#016llx :: %#016llx\n", current_pointer, session->entry_point);

    clock_t start, end;
    double cpu_time_used;
    printf("clock start\n");

    start = clock();

    set_hardware_watchpoint(session, session->entry_point, 0, HW_BREAKPOINT_TYPE_X, 8);

    if (!continue_until_breakpoint(session)) {
        printf("continue failed!\n");
    }

    clear_hardware_watchpoint(session);

    get_state(&session->thread, &state);
    current_pointer = (unsigned long long)state.uts.ts64.__rip;

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time spent :: %lf\n", cpu_time_used);
    printf("Current position : %#016llx\n", current_pointer);
    printf("\n===============================================\n\n");

    // printf("PID :: %d\n",session->pid);
    // sleep(20);

    unsigned long long amountOfInstructions = 0;
    unsigned long long current_stack_pointer, initial_stack_pointer = (unsigned long long)state.uts.ts64.__rsp;

    getLinkedLibraries(session);

    start = clock();
    amountOfInstructions = 0;
    do {
        step(&session->task, &session->thread, &state);
        debug_wait(session->pid);
        get_state(&current_thread, &state);
        current_stack_pointer = (unsigned long long)state.uts.ts64.__rsp;

        if (!is_overable(session, (unsigned long long)state.uts.ts64.__rip)) {

            amountOfInstructions += 1;
            session->instruction_count += 1;
            session->current_state = state;
            if (callback) {
                callback(session);
            }
        }
    } while (current_stack_pointer <= initial_stack_pointer);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time spent :: %lf\n", cpu_time_used);
    printf("Instructions skipped :: %llu\n", amountOfInstructions);
    printf("Current position : %#016llx\n", current_pointer);
}

pid_t spawn_process(char *file_path) {
    int retval;
    pid_t new_pid;
    cpu_type_t cpu = CPU_TYPE_X86_64;
    posix_spawnattr_t attr;

    retval = posix_spawnattr_init (&attr);

    if (retval != 0) printf ("Couldn't initialize attributes for posix_spawn, error: %d", retval);

    int ps_flags = 0;
    ps_flags = POSIX_SPAWN_START_SUSPENDED;
    ps_flags |= _POSIX_SPAWN_DISABLE_ASLR;

    retval = posix_spawnattr_setflags (&attr, ps_flags);
    if (retval != 0) printf ("Couldn't add _POSIX_SPAWN_DISABLE_ASLR and POSIX_SPAWN_START_SUSPENDED to posix_spawn flags, got return value %d", retval);

    size_t copied = 0;
    retval = posix_spawnattr_setbinpref_np(&attr, 1, &cpu, &copied);
    if (retval != 0 || copied != 1) printf ("Couldn't set the binary preferences, error: %d", retval);

    retval = posix_spawnp (&new_pid, file_path, NULL,  &attr, NULL, NULL); //argv, env);
    return new_pid;
}


void handle_error(kern_return_t err, char *type) {
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "%s failed\n", type);
        exit(EXIT_FAILURE);
    }
}

void get_threads(mach_port_t *task, thread_act_array_t *threads) {
   mach_msg_type_number_t threadCount;

   kern_return_t err = task_threads(*task, threads, &threadCount);
   handle_error(err, "task_threads()");
   assert(threadCount > 0);
}

void print_state(x86_thread_state_t *state) {
    printf("%llx -- %d\n", state->uts.ts64.__rip, ++instruction_counter); // RIP
}

void get_state(thread_act_t *thread, x86_thread_state_t *state) {
    mach_msg_type_number_t count = x86_THREAD_STATE_COUNT;

    kern_return_t err = thread_get_state(*thread, x86_THREAD_STATE, (thread_state_t)state, &count);
    handle_error(err, "thread_get_state()");
}

void set_state(thread_act_t *thread, x86_thread_state_t *state) {
    mach_msg_type_number_t count = x86_THREAD_STATE_COUNT;

    kern_return_t err = thread_set_state(*thread, x86_THREAD_STATE, (thread_state_t)state, count);
    handle_error(err, "thread_set_state()");
}

void step(mach_port_t *task, thread_act_t *thread, x86_thread_state_t *state) {
    state->uts.ts64.__rflags = ((state->uts.ts64.__rflags & ~0x100UL) | 0x100UL);
    set_state(thread, state);
    DEBUG_PRINT("Resuming...\n");
    kern_return_t err = task_resume(*task);
    handle_error(err, "task_resume()");
}

bool continue_execution(mach_port_t *task, thread_act_t *thread, x86_thread_state_t *state) {
    state->uts.ts64.__rflags = (state->uts.ts64.__rflags & ~0x100UL);
    set_state(thread, state);
    DEBUG_PRINT("Resuming...\n");
    kern_return_t err = task_resume(*task);
    handle_error(err, "task_resume()");
    return true;
}

bool continue_until_breakpoint(debug_session *session) {
    RDebugReasonType reason, ret = false;
    x86_thread_state_t state;

    while (true) {
        get_state(&session->thread, &state);
        uint64_t address = state.uts.ts64.__rip;

        if (restore_breakpoint_instruction(session, address)) {
            step(&session->task, &session->thread, &state);
            set_breakpoint(session, address);
        }

        ret = continue_execution(&session->task, &session->thread, &state);
        reason = debug_wait(session->pid);

        if (reason == R_DEBUG_REASON_BREAKPOINT) {
            printf("HIT BREAKPOINT!\n");
            break;
        }

        if (reason == R_DEBUG_REASON_DEAD) {
            printf("DEBUG DEAD!\n");
            return false;
        }
    }
    return ret;
}

bool set_breakpoint(debug_session *session, uint64_t address) {
    char break_code = BREAKPOINT_OPCODE;
    char *backup = read_memory(session->task, address, sizeof(char));

    if (write_memory(session->task, address, &break_code, sizeof(char))) {
        breakpoint bp;
        bp.address = address;
        bp.saved_data = *backup;
        list_append(&session->breakpoints, &bp);
        return true;
    }
    return false;
}

bool set_hardware_watchpoint(debug_session *session, uint64_t address, int num, int type, size_t len) {
  struct x86_debug_state state;
  mach_msg_type_number_t count = x86_DEBUG_STATE_COUNT;

  kern_return_t result = thread_get_state(session->thread, x86_DEBUG_STATE, &state, &count);
  if (result != KERN_SUCCESS) return false;

  state.uds.ds64.__dr0 = address;
  state.uds.ds64.__dr7 = (1 << 9) | (1 << 8) | (1 << 0);
  result = thread_set_state(session->thread, x86_DEBUG_STATE, &state, count);
  if (result != KERN_SUCCESS) return false;
  return true;
}

bool clear_hardware_watchpoint(debug_session *session) {
  struct x86_debug_state state;
  mach_msg_type_number_t count = x86_DEBUG_STATE_COUNT;

  kern_return_t result = thread_get_state(session->thread, x86_DEBUG_STATE, &state, &count);
  if (result != KERN_SUCCESS) return false;

  state.uds.ds64.__dr6 &= ~(1 << 0);
  state.uds.ds64.__dr7 &= ~(1 << 0);

  count = x86_DEBUG_STATE_COUNT;
  result = thread_set_state(session->thread, x86_DEBUG_STATE, &state, count);
  if (result != KERN_SUCCESS) return false;
  printf("Breakpoint cleared\n");
  return true;
}

bool restore_breakpoint_instruction(debug_session *session, uint64_t address) {
    char instruction_patch;
    if (!find_breakpoint(&session->breakpoints, address, &instruction_patch)) {
        return false;
    }
    if (!write_memory(session->task, address, &instruction_patch, sizeof(char))) {
        return false;
    }
    return true;
}


bool find_breakpoint(list *breakpoints, uint64_t address, char *result) {
    node *element = breakpoints->head;
    while (element) {
        breakpoint *current = element->data;
        if (current->address == address) {
            *result = current->saved_data;
            return true;
        }
        element = element->next;
    }
    return false;
}

bool is_overable(debug_session *session, unsigned long long address) {
    node *element = session->executable_libs.head;

    while (element) {
        executable_library_info *lib_info = element->data;
        unsigned long long start_address = lib_info->start_address;
        unsigned long long difference = address-start_address;
        unsigned long long image_size = lib_info->image_size;
        if (difference <= image_size) {
            return false;
        }
        element = element->next;
    }
    return true;
}

void attach(pid_t pid) {
    xnu_attach(pid);
}

void detach(pid_t pid) {
    xnu_detach(pid);
}
