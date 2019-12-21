#include "mach_parser.h"

int check_magic(uint32_t magic) {
    return !(magic == MH_MAGIC || magic == MH_CIGAM ||
             magic == MH_MAGIC_64 || magic == MH_CIGAM_64 ||
             magic == FAT_MAGIC || magic == FAT_CIGAM);
}

uint64_t parse_file(char *binary) {
    int fd;
    uint32_t magic_number;

    if ((fd = open(binary, O_RDONLY)) < 0) {
        fprintf(stderr, "[-] could not open() %s...\n", binary);
        return 0LL;
    }

    lseek(fd, 0, SEEK_SET);
    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(stderr, "[-] unable to stat().\n");
        return 0LL;
    }

    void* map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        fprintf(stderr, "[-] could not mmap().\n");
        return 0LL;
    }
    read(fd, &magic_number, sizeof magic_number);
    if (check_magic(magic_number)) {
        fprintf(stderr, "[-] not a mach-o binary file.\n");
        return 0LL;
    }

    int swap = magic_number == MH_CIGAM || magic_number == MH_CIGAM_64;
    void *mh = init_header(binary, map, magic_number, &st, swap);
    uint64_t entry_point = get_entry_point(mh, swap);

    return entry_point;
}

uint64_t get_entry_point(void* mh, int swap) {
    uint64_t entryPoint = 0LL;

    if (((struct mach_header_64*)mh)->magic == FAT_MAGIC || ((struct mach_header_64*)mh)->magic == FAT_CIGAM ) {
        struct fat_header fat_header = *(struct fat_header*)mh;
        uint32_t nfat_arch = fat_header.magic == FAT_CIGAM ? OSSwapInt32(fat_header.nfat_arch) : fat_header.nfat_arch;
        struct fat_arch* arch = (struct fat_arch*)((uint64_t)mh + sizeof(struct fat_header));
        while (nfat_arch--) {
            uint32_t offset = fat_header.magic == FAT_CIGAM ? OSSwapInt32(arch[nfat_arch].offset) : arch[nfat_arch].offset;
            struct mach_header_64* header = (struct mach_header_64*)((uint64_t)mh + offset);
            entryPoint = get_entry_segment(header, swap);
            if (entryPoint != 0LL) {
                return entryPoint;
            }
        }
    } else {
        struct mach_header_64* header = (struct mach_header_64*)mh;
        entryPoint = get_entry_segment(header, swap);
    }
    return entryPoint;
}

uint64_t get_entry_segment(void *mh, int swap) {
    int flag = 0;
    struct mach_header_64* header = (struct mach_header_64*)mh;
    struct load_command* lc = get_load_command((void*)header, swap, 0);

    uint64_t base_address = get_text_segment(lc, header, swap);
    uint64_t offset = 0LL;
    uint64_t entry_point = 0LL;

    for (uint32_t i = 0; i < header->ncmds; i++) {
         if (lc->cmd == LC_MAIN) {
            struct entry_point_command* ep = (struct entry_point_command*)lc;
            if (swap) {
                swap_entry_point_command(ep, -1);
            }
            offset = ep->entryoff + ep->stacksize;
            entry_point = base_address + offset;
            return entry_point;
        }
        lc = (struct load_command*)((uint64_t)lc + lc->cmdsize);
    }
    return entry_point;
}

void* init_header(const char* name, void* map, uint32_t magic_nm, struct stat* st, int swap) {
    void* header;
    void* load_segments;
    mprotect(map, st->st_size, PROT_WRITE);

    if (magic_nm == MH_MAGIC_64 || magic_nm == MH_CIGAM_64) {
        load_segments = (void*)((uint64_t)map + sizeof(struct mach_header_64));
        header = (struct mach_header_64*)dump_mach_header(map, swap, magic_nm);
        memcpy(map, header, sizeof(struct mach_header_64));
        memcpy(map + sizeof(struct mach_header_64), load_segments, st->st_size - sizeof(struct mach_header_64));
    } else if (magic_nm == MH_MAGIC || magic_nm == MH_CIGAM) {
        load_segments = (void*)((uint64_t)map + sizeof(struct mach_header));
        header = (struct mach_header*)dump_mach_header(map, swap, magic_nm);
        memcpy(map, header, sizeof(struct mach_header));
        memcpy(map + sizeof(struct mach_header), load_segments, st->st_size - sizeof(struct mach_header));
    } else {
        header = (struct fat_header*)dump_fat_header(map, swap);
    }

    free(header);

    return map;
}

static struct mach_header_64* dump_mach_header(void* map, int swap, uint32_t magic_nm) {
    char* header = NULL;
    struct mach_header_64* mach_header = malloc(sizeof(struct mach_header_64));
    assert(mach_header);
    memcpy((void*)mach_header, (const void*)map, sizeof(struct mach_header_64));
    if (magic_nm == MH_MAGIC || magic_nm == MH_CIGAM)
        mach_header->reserved = 0;
    if (swap)
        swap_mach_header_64(mach_header, -1);
    struct load_command* lc = get_load_command(map, swap, LC_DYSYMTAB);
    return mach_header;
}

static struct fat_header* dump_fat_header(void* map, int swap) {
    char* header = NULL;
    struct fat_header* fat_header = malloc(sizeof(struct fat_header));
    memcpy((void*)fat_header, map, sizeof(struct fat_header));
    int fat_swap = fat_header->magic == FAT_CIGAM;
    if (fat_swap)
        swap_fat_header(fat_header, -1);
    struct fat_arch* arch = calloc(fat_header->nfat_arch, sizeof(struct fat_arch));
    assert(arch);
    memcpy((void*)arch, (const void*)((uint64_t)map + sizeof(struct fat_header)), sizeof(struct fat_arch) * fat_header->nfat_arch);
    if (fat_swap)
        swap_fat_arch(arch, fat_header->nfat_arch, -1);
    uint32_t nfat_arch = fat_header->nfat_arch;
    while (nfat_arch--) {
        struct mach_header_64* mach_header = (struct mach_header_64*)((uint64_t)map + arch[nfat_arch].offset);
        struct load_command* lc = (struct load_command*)get_load_command((void*)mach_header, swap, LC_DYSYMTAB);
        if (mach_header->magic == MH_CIGAM || mach_header->magic == MH_CIGAM_64) {
            if (mach_header->magic == MH_CIGAM)
                swap_mach_header((struct mach_header*)mach_header, -1);
            else
                swap_mach_header_64(mach_header, -1);
        }
    }
    free(arch);
    return fat_header;
}

static struct load_command* get_load_command(void* mach_header, int swap, uint32_t cmd) {
    struct mach_header_64* header = (struct mach_header_64*)mach_header;
    struct load_command* lc = NULL;
    if (header->magic == MH_MAGIC) {
        lc = (struct load_command*)((uint64_t)((struct mach_header*)header) + sizeof(struct mach_header));
    } else {
        lc = (struct load_command*)((uint64_t)header + sizeof(struct mach_header_64));
    }
    if (cmd) {
        for (uint32_t i = 0; i < header->ncmds; i++) {
            if (lc->cmd == cmd)
                break;
            lc = (struct load_command*)((uint64_t)lc + lc->cmdsize);
        }
    }
    if (swap)
        swap_load_command(lc, -1);
    return lc;
}

static uint64_t get_text_segment(struct load_command* load_cmd, struct mach_header_64* mach_header, int swap) {
    struct load_command* lc = load_cmd;
    size_t sz = 400;
    uint64_t address = 0LL;

    for (uint32_t i = 0; i < mach_header->ncmds; i++) {
        if (lc->cmd == LC_SEGMENT) {
            struct segment_command* seg = (struct segment_command*)lc;
            if (swap)
                swap_segment_command(seg, -1);
            if (!(strcmp(seg->segname, SEG_TEXT))) {
                address = seg->vmaddr;
                return address;
            }
        } else if (lc->cmd == LC_SEGMENT_64) {
            struct segment_command_64* seg = (struct segment_command_64*)lc;
            if (swap)
                swap_segment_command_64(seg, -1);
            if (!(strcmp(seg->segname, SEG_TEXT))) {
                address = seg->vmaddr;
                return address;
            }
        }
        lc = (struct load_command*)((uint64_t)lc + lc->cmdsize);
    }
    return address;
}
