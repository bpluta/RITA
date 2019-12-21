#include "decoder.h"

ZydisDecoder decoder;
ZydisFormatter formatter;

void get_register_value(ZydisRegister reg) { //, x86_thread_state_t *state) { //unsigned long long
    if (!(reg^ZYDIS_REGISTER_AL) || !(reg^ZYDIS_REGISTER_AH) || !(reg^ZYDIS_REGISTER_AX) || !(reg^ZYDIS_REGISTER_EAX) || !(reg^ZYDIS_REGISTER_RAX)) {
        printf(" RAX ");
        // return (unsigned long long)*state.uts.ts64.__rax;
    }

    if (!(reg^ZYDIS_REGISTER_BL) || !(reg^ZYDIS_REGISTER_BH) || !(reg^ZYDIS_REGISTER_BX) || !(reg^ZYDIS_REGISTER_EBX) || !(reg^ZYDIS_REGISTER_RBX)) {
        printf(" RBX ");
        // return (unsigned long long)*state.uts.ts64.__rbx;
    }

    if (!(reg^ZYDIS_REGISTER_CL) || !(reg^ZYDIS_REGISTER_CH) || !(reg^ZYDIS_REGISTER_CX) || !(reg^ZYDIS_REGISTER_ECX) || !(reg^ZYDIS_REGISTER_RCX)) {
        printf(" RCX ");
        // return (unsigned long long)*state.uts.ts64.__rcx;
    }

    if (!(reg^ZYDIS_REGISTER_DL) || !(reg^ZYDIS_REGISTER_DH) || !(reg^ZYDIS_REGISTER_DX) || !(reg^ZYDIS_REGISTER_EDX) || !(reg^ZYDIS_REGISTER_RDX)) {
        printf(" RDX ");
        // return (unsigned long long)*state.uts.ts64.__rdx;
    }

    if (!(reg^ZYDIS_REGISTER_SPL) || !(reg^ZYDIS_REGISTER_SP) || !(reg^ZYDIS_REGISTER_ESP) || !(reg^ZYDIS_REGISTER_RSP)) {
        printf(" RSP ");
        // return (unsigned long long)*state.uts.ts64.__rsp;
    }

    if (!(reg^ZYDIS_REGISTER_BPL) || !(reg^ZYDIS_REGISTER_BP) || !(reg^ZYDIS_REGISTER_EBP) || !(reg^ZYDIS_REGISTER_RBP)) {
        printf(" RBP ");
        // return (unsigned long long)*state.uts.ts64.__rbp;
    }

    if (!(reg^ZYDIS_REGISTER_SIL) || !(reg^ZYDIS_REGISTER_SI) || !(reg^ZYDIS_REGISTER_ESI) || !(reg^ZYDIS_REGISTER_RSI)) {
        printf(" RSI ");
        // return (unsigned long long)*state.uts.ts64.__rsi;
    }

    if (!(reg^ZYDIS_REGISTER_DIL) || !(reg^ZYDIS_REGISTER_DI) || !(reg^ZYDIS_REGISTER_EDI) || !(reg^ZYDIS_REGISTER_RDI)) {
        printf(" RDI ");
        // return (unsigned long long)*state.uts.ts64.__rdi;
    }

    if (!(reg^ZYDIS_REGISTER_R8B) || !(reg^ZYDIS_REGISTER_R8W) || !(reg^ZYDIS_REGISTER_R8D) || !(reg^ZYDIS_REGISTER_R8)) {
        printf(" R8 ");
        // return (unsigned long long)*state.uts.ts64.__r8;
    }

    if (!(reg^ZYDIS_REGISTER_R9B) || !(reg^ZYDIS_REGISTER_R9W) || !(reg^ZYDIS_REGISTER_R9D) || !(reg^ZYDIS_REGISTER_R9)) {
        printf(" R9 ");
        // return (unsigned long long)*state.uts.ts64.__r9;
    }

    if (!(reg^ZYDIS_REGISTER_R10B) || !(reg^ZYDIS_REGISTER_R10W) || !(reg^ZYDIS_REGISTER_R10D) || !(reg^ZYDIS_REGISTER_R10)) {
        printf(" R10 ");
        // return (unsigned long long)*state.uts.ts64.__r10;
    }

    if (!(reg^ZYDIS_REGISTER_R11B) || !(reg^ZYDIS_REGISTER_R11W) || !(reg^ZYDIS_REGISTER_R11D) || !(reg^ZYDIS_REGISTER_R11)) {
        printf(" R11 ");
        // return (unsigned long long)*state.uts.ts64.__r11;
    }

    if (!(reg^ZYDIS_REGISTER_R12B) || !(reg^ZYDIS_REGISTER_R12W) || !(reg^ZYDIS_REGISTER_R12D) || !(reg^ZYDIS_REGISTER_R12)) {
        printf(" R12 ");
        // return (unsigned long long)*state.uts.ts64.__r12;
    }

    if (!(reg^ZYDIS_REGISTER_R13B) || !(reg^ZYDIS_REGISTER_R13W) || !(reg^ZYDIS_REGISTER_R13D) || !(reg^ZYDIS_REGISTER_R13)) {
        printf(" R13 ");
        // return (unsigned long long)*state.uts.ts64.__r13;
    }

    if (!(reg^ZYDIS_REGISTER_R14B) || !(reg^ZYDIS_REGISTER_R14W) || !(reg^ZYDIS_REGISTER_R14D) || !(reg^ZYDIS_REGISTER_R14)) {
        printf(" R14 ");
        // return (unsigned long long)*state.uts.ts64.__r14;
    }

    if (!(reg^ZYDIS_REGISTER_R15B) || !(reg^ZYDIS_REGISTER_R15W) || !(reg^ZYDIS_REGISTER_R15D) || !(reg^ZYDIS_REGISTER_R15)) {
        printf(" R15 ");
        // return (unsigned long long)*state.uts.ts64.__r15;
    }

    if (!(reg^ZYDIS_REGISTER_IP) || !(reg^ZYDIS_REGISTER_EIP) || !(reg^ZYDIS_REGISTER_RIP)) {
        printf(" RIP ");
        // return (unsigned long long)*state.uts.ts64.__rip;
    }

    if (!(reg^ZYDIS_REGISTER_RFLAGS)) {
        printf(" RFLAGS ");
        // return (unsigned long long)*state.uts.ts64.__rflags;
    }

    if (!(reg^ZYDIS_REGISTER_CS)) {
        printf(" CS ");
        // return (unsigned long long)*state.uts.ts64.__rflags;
    }

    if (!(reg^ZYDIS_REGISTER_FS)) {
        printf(" FS ");
        // return (unsigned long long)*state.uts.ts64.__rflags;
    }

    if (!(reg^ZYDIS_REGISTER_GS)) {
        printf(" GS ");
        // return (unsigned long long)*state.uts.ts64.__rflags;
    }
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
    long long reg = operand.reg.value;
    if (reg != 0 && reg) {
        registers->append(registers,reg);
    }
}

void decoder_init() {
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

void decode(unsigned long long rip, unsigned long *data, void *buffer, size_t size, x86_thread_state_t *state, register_buffer *registers) {
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
            }
        }
    } else {
        printf("Could not decode instruction\n");
    }
}
