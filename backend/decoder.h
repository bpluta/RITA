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
#include "list.h"
#include "process.h"
#include "session.h"

typedef enum _register_type {
    REGISTER_NONE,
    REGISTER_RAX,
    REGISTER_RBX,
    REGISTER_RCX,
    REGISTER_RDX,
    REGISTER_RSP,
    REGISTER_RBP,
    REGISTER_RSI,
    REGISTER_RDI,
    REGISTER_R8,
    REGISTER_R9,
    REGISTER_R10,
    REGISTER_R11,
    REGISTER_R12,
    REGISTER_R13,
    REGISTER_R14,
    REGISTER_R15,
    REGISTER_RIP,
    REGISTER_RFLAGS,
    REGISTER_CS,
    REGISTER_FS,
    REGISTER_GS
} register_type;

void decoder_init();
char *get_register_name(register_type reg);
register_type get_register_value(ZydisRegister reg);
uint64_t get_register_val(debug_session *session, register_type reg);
void get_operand_action(ZydisDecodedOperand operand);
void get_operand_type(ZydisDecodedOperand operand);
void get_memory_type(ZydisDecodedOperand operand);
void get_immediate(ZydisDecodedOperand operand);
void get_pointer(ZydisDecodedOperand operand);
void get_register(ZydisDecodedOperand operand, register_buffer *registers);
void decode(debug_session *session, unsigned long long rip, unsigned long *data, void *buffer, size_t size, x86_thread_state_t *state, register_buffer *registers, list *memory);

#endif
