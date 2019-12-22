#include "decoder.h"

ZydisDecoder decoder;
ZydisFormatter formatter;

char *get_register_name(register_type reg) {
    switch (reg) {
        case REGISTER_RAX:
            return "RAX";
        case REGISTER_RBX:
            return "RBX";
        case REGISTER_RCX:
            return "RCX";
        case REGISTER_RDX:
            return "RDX";
        case REGISTER_RSP:
            return "RSP";
        case REGISTER_RBP:
            return "RBP";
        case REGISTER_RSI:
            return "RSI";
        case REGISTER_RDI:
            return "RDI";
        case REGISTER_R8:
            return "R8";
        case REGISTER_R9:
            return "R9";
        case REGISTER_R10:
            return "R10";
        case REGISTER_R11:
            return "R11";
        case REGISTER_R12:
            return "R12";
        case REGISTER_R13:
            return "R13";
        case REGISTER_R14:
            return "R14";
        case REGISTER_R15:
            return "R15";
        case REGISTER_RIP:
            return "RIP";
        case REGISTER_RFLAGS:
            return "RFLAGS";
        case REGISTER_CS:
            return "CS";
        case REGISTER_FS:
            return "FS";
        case REGISTER_GS:
            return "GS";
    }
    return "";
}

uint64_t get_register_val(debug_session *session, register_type reg) {
    switch (reg) {
        case REGISTER_RAX:
            return (uint64_t)session->current_state.uts.ts64.__rax;
        case REGISTER_RBX:
            return (uint64_t)session->current_state.uts.ts64.__rbx;
        case REGISTER_RCX:
            return (uint64_t)session->current_state.uts.ts64.__rcx;
        case REGISTER_RDX:
            return (uint64_t)session->current_state.uts.ts64.__rdx;
        case REGISTER_RSP:
            return (uint64_t)session->current_state.uts.ts64.__rsp;
        case REGISTER_RBP:
            return (uint64_t)session->current_state.uts.ts64.__rbp;
        case REGISTER_RSI:
            return (uint64_t)session->current_state.uts.ts64.__rsi;
        case REGISTER_RDI:
            return (uint64_t)session->current_state.uts.ts64.__rdi;
        case REGISTER_R8:
            return (uint64_t)session->current_state.uts.ts64.__r8;
        case REGISTER_R9:
            return (uint64_t)session->current_state.uts.ts64.__r9;
        case REGISTER_R10:
            return (uint64_t)session->current_state.uts.ts64.__r10;
        case REGISTER_R11:
            return (uint64_t)session->current_state.uts.ts64.__r11;
        case REGISTER_R12:
            return (uint64_t)session->current_state.uts.ts64.__r12;
        case REGISTER_R13:
            return (uint64_t)session->current_state.uts.ts64.__r13;
        case REGISTER_R14:
            return (uint64_t)session->current_state.uts.ts64.__r14;
        case REGISTER_R15:
            return (uint64_t)session->current_state.uts.ts64.__r15;
        case REGISTER_RIP:
            return (uint64_t)session->current_state.uts.ts64.__rip;
        case REGISTER_RFLAGS:
            return (uint64_t)session->current_state.uts.ts64.__rflags;
        case REGISTER_CS:
            return (uint64_t)session->current_state.uts.ts64.__cs;
        case REGISTER_FS:
            return (uint64_t)session->current_state.uts.ts64.__fs;
        case REGISTER_GS:
            return (uint64_t)session->current_state.uts.ts64.__gs;
    }
    return 0LL;
}

register_type get_register_value(ZydisRegister reg) {
    if (!(reg^ZYDIS_REGISTER_AL) || !(reg^ZYDIS_REGISTER_AH) || !(reg^ZYDIS_REGISTER_AX) || !(reg^ZYDIS_REGISTER_EAX) || !(reg^ZYDIS_REGISTER_RAX)) {
        return REGISTER_RAX;
    }

    if (!(reg^ZYDIS_REGISTER_BL) || !(reg^ZYDIS_REGISTER_BH) || !(reg^ZYDIS_REGISTER_BX) || !(reg^ZYDIS_REGISTER_EBX) || !(reg^ZYDIS_REGISTER_RBX)) {
        return REGISTER_RBX;
    }

    if (!(reg^ZYDIS_REGISTER_CL) || !(reg^ZYDIS_REGISTER_CH) || !(reg^ZYDIS_REGISTER_CX) || !(reg^ZYDIS_REGISTER_ECX) || !(reg^ZYDIS_REGISTER_RCX)) {
        return REGISTER_RCX;
    }

    if (!(reg^ZYDIS_REGISTER_DL) || !(reg^ZYDIS_REGISTER_DH) || !(reg^ZYDIS_REGISTER_DX) || !(reg^ZYDIS_REGISTER_EDX) || !(reg^ZYDIS_REGISTER_RDX)) {
        return REGISTER_RDX;
    }

    if (!(reg^ZYDIS_REGISTER_SPL) || !(reg^ZYDIS_REGISTER_SP) || !(reg^ZYDIS_REGISTER_ESP) || !(reg^ZYDIS_REGISTER_RSP)) {
        return REGISTER_RSP;
    }

    if (!(reg^ZYDIS_REGISTER_BPL) || !(reg^ZYDIS_REGISTER_BP) || !(reg^ZYDIS_REGISTER_EBP) || !(reg^ZYDIS_REGISTER_RBP)) {
        return REGISTER_RSP;
    }

    if (!(reg^ZYDIS_REGISTER_SIL) || !(reg^ZYDIS_REGISTER_SI) || !(reg^ZYDIS_REGISTER_ESI) || !(reg^ZYDIS_REGISTER_RSI)) {
        return REGISTER_RSI;
    }

    if (!(reg^ZYDIS_REGISTER_DIL) || !(reg^ZYDIS_REGISTER_DI) || !(reg^ZYDIS_REGISTER_EDI) || !(reg^ZYDIS_REGISTER_RDI)) {
        return REGISTER_RDI;
    }

    if (!(reg^ZYDIS_REGISTER_R8B) || !(reg^ZYDIS_REGISTER_R8W) || !(reg^ZYDIS_REGISTER_R8D) || !(reg^ZYDIS_REGISTER_R8)) {
        return REGISTER_R8;
    }

    if (!(reg^ZYDIS_REGISTER_R9B) || !(reg^ZYDIS_REGISTER_R9W) || !(reg^ZYDIS_REGISTER_R9D) || !(reg^ZYDIS_REGISTER_R9)) {
        return REGISTER_R9;
    }

    if (!(reg^ZYDIS_REGISTER_R10B) || !(reg^ZYDIS_REGISTER_R10W) || !(reg^ZYDIS_REGISTER_R10D) || !(reg^ZYDIS_REGISTER_R10)) {
        return REGISTER_R10;
    }

    if (!(reg^ZYDIS_REGISTER_R11B) || !(reg^ZYDIS_REGISTER_R11W) || !(reg^ZYDIS_REGISTER_R11D) || !(reg^ZYDIS_REGISTER_R11)) {
        return REGISTER_R11;
    }

    if (!(reg^ZYDIS_REGISTER_R12B) || !(reg^ZYDIS_REGISTER_R12W) || !(reg^ZYDIS_REGISTER_R12D) || !(reg^ZYDIS_REGISTER_R12)) {
        return REGISTER_R12;
    }

    if (!(reg^ZYDIS_REGISTER_R13B) || !(reg^ZYDIS_REGISTER_R13W) || !(reg^ZYDIS_REGISTER_R13D) || !(reg^ZYDIS_REGISTER_R13)) {
        return REGISTER_R13;
    }

    if (!(reg^ZYDIS_REGISTER_R14B) || !(reg^ZYDIS_REGISTER_R14W) || !(reg^ZYDIS_REGISTER_R14D) || !(reg^ZYDIS_REGISTER_R14)) {
        return REGISTER_R14;
    }

    if (!(reg^ZYDIS_REGISTER_R15B) || !(reg^ZYDIS_REGISTER_R15W) || !(reg^ZYDIS_REGISTER_R15D) || !(reg^ZYDIS_REGISTER_R15)) {
        return REGISTER_R15;
    }

    if (!(reg^ZYDIS_REGISTER_IP) || !(reg^ZYDIS_REGISTER_EIP) || !(reg^ZYDIS_REGISTER_RIP)) {
        return REGISTER_RIP;
    }

    if (!(reg^ZYDIS_REGISTER_RFLAGS)) {
        return REGISTER_RFLAGS;
    }

    if (!(reg^ZYDIS_REGISTER_CS)) {
        return REGISTER_CS;
    }

    if (!(reg^ZYDIS_REGISTER_FS)) {
        return REGISTER_FS;
    }

    if (!(reg^ZYDIS_REGISTER_GS)) {
        return REGISTER_GS;
    }

    return REGISTER_NONE;
}

void get_operand_action(ZydisDecodedOperand operand) {
    ZyanU8 action = operand.actions;
    if (action == ZYDIS_OPERAND_ACTION_READ) {
        printf("[READ] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_WRITE) {
        printf("[WRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_CONDREAD) {
        printf("[CONDREAD] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_CONDWRITE) {
        printf("[CONDWRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_READWRITE) {
        printf("[READWRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_CONDREAD_CONDWRITE) {
        printf("[CONDREAD CONDWRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_READ_CONDWRITE) {
        printf("[READ CONDWRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_CONDREAD_WRITE) {
        printf("[CONDREAD WRITE] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_MASK_READ) {
        printf("[MASK READ] ");
    }
    if (action == ZYDIS_OPERAND_ACTION_MASK_WRITE) {
        printf("[MASK WRITE] ");
    }
}

void get_operand_type(ZydisDecodedOperand operand) {
    ZyanU8 action = operand.type;
    if (action == ZYDIS_OPERAND_TYPE_UNUSED) {
        printf("[UNUSED] ");
    }
    if (action == ZYDIS_OPERAND_TYPE_REGISTER) {
        printf("[REGISTER]");
    }
    if (action == ZYDIS_OPERAND_TYPE_MEMORY) {
        printf("[MEMORY] ");
    }
    if (action == ZYDIS_OPERAND_TYPE_POINTER) {
        printf("[POINTER] ");
    }
    if (action == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
        printf("[IMMEDIATE] ");
    }
}

void get_memory_type(ZydisDecodedOperand operand) {
    ZydisMemoryOperandType memType = operand.mem.type;
    if (memType == ZYDIS_MEMOP_TYPE_INVALID) {
        // printf("[INVALID] ");
    }
    if (memType == ZYDIS_MEMOP_TYPE_MEM) {
        printf("[MEM] ");
    }
    if (memType == ZYDIS_MEMOP_TYPE_AGEN) {
        printf("[AGEN] ");
    }
    if (memType == ZYDIS_MEMOP_TYPE_MIB) {
        printf("[MIB] ");
    }
}

void get_immediate(ZydisDecodedOperand operand) {
    long long u = operand.imm.value.u;
    long long s = operand.imm.value.s;
    if (u != 0) {
        printf("%#016llx ", u);
    }
    if (s != 0) {
        printf("%#016llx ", s);
    }
}

void get_pointer(ZydisDecodedOperand operand) {
    long long segment = operand.ptr.segment;
    long long offset = operand.ptr.offset;
    if (segment != 0) {
        printf("%#016llx ", segment);
    }
    if (offset != 0) {
        printf("%#016llx ", offset);
    }
}

void get_register(ZydisDecodedOperand operand, register_buffer *registers) {
    if (operand.reg.value) {
        register_type reg = get_register_value(operand.reg.value);
        registers->append(registers,reg);
    }
}

void get_memory(debug_session *session, ZydisDecodedOperand operand) {
    if (!operand.mem.segment) return;

    uint64_t base = 0;
    uint64_t index = 0;

    if (operand.mem.base) {
        register_type reg = get_register_value(operand.mem.base);
        if (reg) {
            base = get_register_val(session, reg);
        }
    }
    if (operand.mem.index) {
        register_type reg = get_register_value(operand.mem.index);
        if (reg) {
            index = get_register_val(session, reg);
        }
    }

    uint8_t scale = operand.mem.scale;
    int64_t displacement = operand.mem.disp.has_displacement ? operand.mem.disp.value : 0;

    uint64_t address = base + (index * scale) + displacement;
    if (address) list_append(&session->recently_modified_memory, &address);
}

void decoder_init() {
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

void decode(debug_session *session, unsigned long long rip, unsigned long *data, void *buffer, size_t size, x86_thread_state_t *state, register_buffer *registers, list *memory) {

    ZyanUSize offset = 0;
    const ZyanUSize length = 15;
    ZydisDecodedInstruction instruction;

    if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data, length, &instruction))) {
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, size, rip);

        printf("%016" PRIX64 "  ", rip); // RIP
        puts(buffer);

        for (ZyanU8 i = 0; i < instruction.operand_count; ++i) {
            ZydisDecodedOperand operand = instruction.operands[i];
            if (operand.actions & ZYDIS_OPERAND_ACTION_MASK_WRITE) {
                get_register(operand, registers);
                get_memory(session, operand);
            }
        }
    } else {
        printf("Could not decode instruction\n");
    }
}
