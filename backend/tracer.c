#include "run.h"
#include "mach_parser.h"
#include "dyld_data.h"
#include "decoder.h"
#include "encoder.h"
#include "process.h"

#define LOGFILE_PATH "./traces/trace"

void get_saved_registers(debug_session *session) {
    register_buffer *registers = &session->recently_modified_registers;
    if (!registers->count) return;
    for (int i=0; i<registers->count; i++) {
        uint64_t value = get_register_value(session, registers->data[i]);
        uint8_t register_type = registers->data[i];
        printf("\t[ REGISTER ] %s -> %llx\n", get_register_name(register_type), value);
        log_register(session->instruction_count, register_type, value);
    }
    registers->clear(registers);
}

void get_saved_memory(debug_session *session) {
    if (!session->recently_modified_memory.length) return;
    node *element = session->recently_modified_memory.head;
    while (element) {
        uint64_t address = *(uint64_t*)element->data;
        uint64_t value = *read_memory(session->task, (mach_vm_address_t) address, sizeof(uint64_t));
        printf("\t[ MEMORY ] %llx -> %llx\n", address, value);
        log_memory(session->instruction_count, address, value);
        element = element->next;
    }
    delete_list(&session->recently_modified_memory);
}

void get_instruction(debug_session *session) {
    session->recently_modified_registers.append(&session->recently_modified_registers, REGISTER_RIP);
    get_saved_registers(session);
    get_saved_memory(session);
    unsigned long long rip = session->current_instruction_pointer;

    uintptr_t address = read_memory(session->task, (mach_vm_address_t) rip, INSTRUCTION_MAX_SIZE);
    if (!address) {
        printf("mach_vm_read failed\n");
        return;
    }
    int buffer_size = 256;
    char buffer[buffer_size];
    decode_instruction(session, rip,((unsigned long long *)(address)), &buffer, buffer_size);
    log_instruction(session->instruction_count, (void *)(address));
    log_flush();
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
