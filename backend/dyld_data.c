#include "dyld_data.h"

size_t size_of_image(struct mach_header *header) {
    size_t sz = sizeof(*header);
    sz += header->sizeofcmds;

    struct load_command *lc = (struct load_command *) (header + 1);
    for (uint32_t i = 0; i < header->ncmds; i++) {
        if (lc->cmd == LC_SEGMENT) {
            sz += ((struct segment_command *) lc)->vmsize;
        }
        lc = (struct load_command *) ((char *) lc + lc->cmdsize);
    }
    return sz;
}

bool get_library_info(debug_session *session, executable_library_info *output, struct dyld_image_info *info, uint64_t slide) {
    mach_msg_type_number_t size = sizeof(struct mach_header_64);
    struct mach_header_64 *header = (struct mach_header_64*)read_memory(session->task, (mach_vm_address_t) info->imageLoadAddress, size);
    uint64_t image_address = info->imageLoadAddress;
    size_t image_size = size_of_image(header);
    uint8_t* image_path = read_memory(session->task, (mach_vm_address_t) info->imageFilePath, PATH_MAX);

    char *dot = strrchr(image_path, '.');
    if (dot && !((strlen(dot) == 6) && !strcmp(dot, ".dylib"))) {
        output->path = image_path;
        output->start_address = image_address;
        output->end_address = image_address + image_size;
        output->image_size = image_size;
        return true;
    }
    return false;
}

bool get_text_segment(debug_session *session, executable_library_info *output, struct dyld_image_info* info, uint64_t slide) {
    mach_msg_type_number_t size = sizeof(struct mach_header_64);
    struct mach_header_64 *header = (struct mach_header_64*)read_memory(session->task, (mach_vm_address_t) info->imageLoadAddress, size);
    size_t image_size = size_of_image(header);

    struct segment_command_64 *sgp;
    struct section_64 *sp;
    sgp = (struct segment_command_64 *)((char *)info->imageLoadAddress + sizeof(struct mach_header_64));
    for (unsigned long i=0; i<header->ncmds; i++) {
      size_t seg_size = sizeof(struct segment_command_64);
      struct segment_command_64 *segment = read_memory(session->task, sgp, seg_size);
      sp = (struct section_64 *)((char *)sgp + sizeof(struct segment_command_64));
      if (strncmp(segment->segname, "__TEXT", sizeof(segment->segname)) == 0) {
        output->path = segment->segname;
        output->start_address = segment->vmaddr + slide;
        output->end_address = segment->vmaddr + segment->vmsize + slide;
        output->image_size = segment->vmsize;
          return true;
      }
      sgp = (struct segment_command_64 *)((char *)sgp + segment->cmdsize);
    }
    return false;
}

bool get_text_section(debug_session *session, executable_library_info *output, struct dyld_image_info* info, uint64_t slide) {
    mach_msg_type_number_t size = sizeof(struct mach_header_64);
    struct mach_header_64 *header = (struct mach_header_64*)read_memory(session->task, (mach_vm_address_t) info->imageLoadAddress, size);
    size_t image_size = size_of_image(header);

    struct segment_command_64 *sgp;
    struct section_64 *sp;
    sgp = (struct segment_command_64 *)((char *)info->imageLoadAddress + sizeof(struct mach_header_64));
    for (unsigned long i=0; i<header->ncmds; i++) {
      size_t seg_size = sizeof(struct segment_command_64);
      struct segment_command_64 *segment = read_memory(session->task, sgp, seg_size);
      sp = (struct section_64 *)((char *)sgp + sizeof(struct segment_command_64));
      if (strncmp(segment->segname, "__TEXT", sizeof(segment->segname)) == 0) {
        for (unsigned long j = 0; j<segment->nsects; j++) {
            size_t sec_size = sizeof(struct section_64);
            struct section_64 *section = read_memory(session->task, sp, sec_size);

            printf("\t%s\n",section->sectname);
            sp = (struct section_64 *)((char *)sp + sizeof(struct section_64));
        }

        output->path = segment->segname;
        output->start_address = segment->vmaddr;
        output->end_address = segment->vmaddr + segment->vmsize ;
        output->image_size = segment->vmsize;
          return true;
      }
      sgp = (struct segment_command_64 *)((char *)sgp + segment->cmdsize);
    }
    return false;
}

void getLinkedLibraries(debug_session *session) {
    struct task_dyld_info dyld_info;
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;

    printf("task_info\n");
    if (task_info(session->task, TASK_DYLD_INFO, (task_info_t) &dyld_info, &count) == KERN_SUCCESS) {
        mach_msg_type_number_t size = sizeof(struct dyld_all_image_infos);
        uint8_t* data = read_memory(session->task, dyld_info.all_image_info_addr, size);
        struct dyld_all_image_infos* infos = (struct dyld_all_image_infos *) data;

        mach_msg_type_number_t size2 = sizeof(struct dyld_image_info) * infos->infoArrayCount;
        uint8_t* info_addr = read_memory(session->task, (mach_vm_address_t) infos->infoArray, size2);
        struct dyld_image_info* info = (struct dyld_image_info*) info_addr;
        uint64_t slide = infos->sharedCacheSlide;

        mach_msg_type_number_t size4 = sizeof(struct mach_header_64);
        printf("address: %llx\n",infos->dyldImageLoadAddress);
        struct mach_header_64 *header = (struct mach_header_64*)read_memory(session->task, (mach_vm_address_t) infos->dyldImageLoadAddress, size4);
        uint32_t ncmds = header->ncmds;
        printf("cmds: %d\n",header->ncmds);

        printf("count: %d\n",infos->infoArrayCount);
        int amount_of_libs = 0;
        for (int i=0; i < infos->infoArrayCount; i++) {
            executable_library_info lib_info;
            uint8_t* image_path = read_memory(session->task, (mach_vm_address_t) info[i].imageFilePath, PATH_MAX);
            if (image_path) {
              char *dot = strrchr(image_path, '.');
              if (dot && !((strlen(dot) == 6) && !strcmp(dot, ".dylib"))) {
                printf("PATH :: %s\n", image_path);
                if (get_text_section(session, &lib_info, &info[i], slide)) {
                  printf("address :: %llx :: %llx :: SIZE %llx\n", lib_info.start_address,lib_info.end_address, lib_info.image_size);
                  printf("\n");
                  session->path = image_path;
                  list_append(&session->executable_libs, &lib_info);
                  amount_of_libs++;
                }
              }
            }
        }
        printf("amount of libs :: %d\n", amount_of_libs);
    }
}
