#include "process.h"
#include <stdio.h>

char *read_memory(task_t task, mach_vm_address_t address, mach_msg_type_number_t size) {
    mach_msg_type_number_t data_size = (mach_msg_type_number_t)size;
    vm_offset_t data;

    kern_return_t error = vm_read(task, address, size, &data, &data_size);

    if (error != KERN_SUCCESS) {
        return NULL;
    }
    return (char *)data;
}

bool write_memory(task_t task, mach_vm_address_t address, void *data, mach_msg_type_number_t size) {
    kern_return_t error = vm_write(task, address, (vm_offset_t)data, size);
    if (error != KERN_SUCCESS) {
        return false;
    }
    return true;
}
