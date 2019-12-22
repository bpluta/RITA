#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define INSTRUCTION_MAX_SIZE 15

#define INSTRUCTION_MASK 1
#define REGISTER_MASK 2
#define MEMORY_MASK 4

void log_init(char* filename);
void log_close();
void log_instruction(int number, void *instruction);
void log_register(int number, uint8_t register_type, uint64_t value);
void log_memory(int number, uint64_t address, uint64_t value);

#endif
