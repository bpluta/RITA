#ifndef DYLD_DATA_H
#define DYLD_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <mach-o/dyld_images.h>
#include <mach/vm_map.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/user.h>

#include <mach/mach.h>
#include <mach-o/getsect.h>

#include <spawn.h>
#include <mach/machine/vm_types.h>

#include "process.h"
#include "session.h"

#define PATH_MAX 2048

typedef struct _executable_library_info {
    char* path;
    uint64_t start_address;
    uint64_t end_address;
    uint64_t image_size;
} executable_library_info;

void getLinkedLibraries(debug_session *session);

#endif
