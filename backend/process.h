#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <mach/mach.h>
#include <mach/vm_map.h>

char *read_memory(task_t task, mach_vm_address_t address, mach_msg_type_number_t size);
bool write_memory(task_t task, mach_vm_address_t address, void *data, mach_msg_type_number_t size);

#endif
