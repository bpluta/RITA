#ifndef MACH_PARSER_H
#define MACH_PARSER_H

#include <assert.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <mach-o/arch.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/swap.h>

int check_magic(uint32_t magic);
uint64_t parse_file(char *binary);
uint64_t get_entry_point(void* mh, int swap);
uint64_t get_entry_segment(void *mh, int swap);
void* init_header(const char* name, void* map, uint32_t magic_nm, struct stat* st, int swap);
static struct mach_header_64* dump_mach_header(void* map, int swap, uint32_t magic_nm);
static struct fat_header* dump_fat_header(void* map, int swap);
static struct load_command* get_load_command(void* mach_header, int swap, uint32_t cmd);
static uint64_t get_text_segment(struct load_command* load_cmd, struct mach_header_64* mach_header, int swap);

#endif
