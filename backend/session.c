#include "session.h"

void init_debug_session(debug_session *session) {
    session->instruction_count = 0;
    session->recently_modified_registers = register_buffer_init();
    new_list(&session->breakpoints, sizeof(breakpoint));
    new_list(&session->executable_libs, sizeof(executable_library_info));

}

void delete_debug_session(debug_session *session) {
    session->recently_modified_registers.free(&session->recently_modified_registers);
    delete_list(&session->breakpoints);
    delete_list(&session->executable_libs);
}
