#include "encoder.h"

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
}

void log_register(int number, uint8_t register_type, uint64_t value) {
    char type = REGISTER_MASK;
    char null_byte = 0;

    fwrite(&number, 4, 1, log_file);
    fwrite(&type, 1, 1, log_file);
    fwrite(&register_type, 1, 1, log_file);
    fwrite(&value, 8, 1, log_file);
    for (int i=0; i<7; i++) {
        fwrite(&null_byte, 1, 1, log_file);
    }
}

void log_memory(int number, uint64_t address, uint64_t value) {
    char type = MEMORY_MASK;
    char null_byte = 0;

    fwrite(&number, 4, 1, log_file);
    fwrite(&type, 1, 1, log_file);
    fwrite(&address, 8, 1, log_file);
    fwrite(&value, 8, 1, log_file);
}

void log_flush() {
    fflush(log_file);
}
