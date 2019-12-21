#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <sys/stat.h>

#define INSTRUCTION_MASK 1
#define REGISTER_MASK 2
#define MEMORY_MASK 4

void log_init(char* filename);
void log_close();
void log_instruction(int number, void *instruction);

#endif
