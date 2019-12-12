#include <stdio.h>
#include <stdlib.h>
#include <Zydis/Zydis.h>

#include "decoder.h"

#define INSTRUCTION_LENGTH 15

static ZydisDecoder decoder;
static ZydisFormatter formatter;

void init() {
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

void decodeInstruction(char *data, long long rip, char *buffer, size_t size) {
    ZydisDecodedInstruction instruction;
    if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data, INSTRUCTION_LENGTH, &instruction))) {
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, size, ZYDIS_RUNTIME_ADDRESS_NONE);
    }
}
