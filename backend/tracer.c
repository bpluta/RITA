#include "run.h"
#include "mach_parser.h"
#include "dyld_data.h"
#include "decoder.h"
#include "encoder.h"

#define LOGFILE_PATH "./traces/trace"

void get_saved_registers(register_buffer *registers) {
    if (!registers->count) { return; }
    printf("\t::");
    for (int i=0; i<registers->count; i++) {
        get_register_value(registers->data[i]);
    }
    printf("\n");
    registers->clear(registers);
}

void get_instruction(debug_session *session) {
    get_saved_registers(&session->recently_modified_registers);
    unsigned long long rip = session->current_state.uts.ts64.__rip;
    uintptr_t address = 0;

    mach_msg_type_number_t num = 15;
    kern_return_t err = vm_read(session->task, rip, 15, &address, &num);
    if (err != KERN_SUCCESS) {
        printf("mach_vm_read failed\n");
        return;
    }
    int buffer_size = 256;
    char buffer[buffer_size];
    decode(rip,((unsigned long long *)(address)), &buffer, buffer_size, &session->current_state, &session->recently_modified_registers);
    log_instruction(session->instruction_count, (void *)(address));
}

int main(int argc, char* argv[]) {
    debug_session session;
    init_debug_session(&session);
    decoder_init();
    log_init(LOGFILE_PATH);

    uint64_t entry_point = parse_file(argv[1]);
    printf("ENTRY 0x%016llx\n", entry_point);
    session.entry_point = entry_point;

    pid_t pid = spawn_process(argv[1]);
    session.pid = pid;

    sleep(1);

    attach(pid);
    printf("About to debug\n");
    task_t task;
    task_for_pid(mach_task_self(),pid, &task);
    session.task = task;

    debug(&session, get_instruction);
    printf("Finished debugging, About to deinitialize\n");

    detach(pid);
    log_close();
    delete_debug_session(&session);
    printf("Finished deinitializing, closing...\n");
    kill(pid,SIGKILL);

    return 0;
}
