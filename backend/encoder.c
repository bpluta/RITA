#include <stdio.h>
#include <sys/stat.h>

#define INSTRUCTION_MASK 1
#define REGISTER_MASK 2
#define MEMORY_MASK 4

static FILE *log_file;

void log_init(char* filename) {
    remove(filename);
    log_file = fopen(filename, "w");
    chmod(filename, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
}

void log_close() {
    fclose(log_file);
}

void log_instruction(int number, void *instruction) {
    char type = INSTRUCTION_MASK;
    char null_byte = 0;
    
    fwrite(&number, 4, 1, log_file);
    fwrite(&type, 1, 1, log_file);
    fwrite(instruction, 15, 1, log_file);
    fwrite(&null_byte, 1, 1, log_file);
    // fclose(log_file);
}
//
// void log_register(int number, void *register, long long value) {
//     char type = INSTRUCTION_MASK;
//     fwrite(&number, 4, sizeof(number), log_file);
//     fwrite(&type, 1, sizeof(curr_char), log_file);
//     fwrite(instruction, 15, sizeof(curr_char), log_file);
// }
