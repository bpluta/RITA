#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <inttypes.h>
#include <mach/mach.h>
#include <Zydis/Zydis.h>
#include <Zycore/Defines.h>
#include <Zycore/Types.h>
#include <Zydis/SharedTypes.h>
#include <Zydis/ShortString.h>

#include "types.h"

void decoder_init();
void get_register_value(ZydisRegister reg);
void get_operand_action(ZydisDecodedOperand operand);
void get_operand_type(ZydisDecodedOperand operand);
void get_memory_type(ZydisDecodedOperand operand);
void get_immediate(ZydisDecodedOperand operand);
void get_pointer(ZydisDecodedOperand operand);
void get_register(ZydisDecodedOperand operand, register_buffer *registers);
void decode(unsigned long long rip, unsigned long *data, void *buffer, size_t size, x86_thread_state_t *state, register_buffer *registers);

#endif
