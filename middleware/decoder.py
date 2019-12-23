import struct

from ctypes import *
from trace_handler import *
from register import *
from commit import *

libdecoder = CDLL("./lib/build/libdecoder.dylib")

INS_MASK = 0b0001
REG_MASK = 0b0010
MEM_MASK = 0b0100

INSTRUCTION_BUFFER_SIZE = 256

TYPE_RANGE = slice(0, 1)

INSTRUCTION_RANGE = slice(1,16)

REGISTER_TYPE_RANGE = slice(1,2)
REGISTER_VALUE_RANGE = slice(2,10)

MEMORY_ADDRESS_RANGE = slice(1,9)
MEMORY_VALUE_RANGE = slice(9,17)

class Decoder:
    def __init__(self):
        libdecoder.init();

    def decodeCommit(self, commit):
        decodedCommit = Commit(commit.index)
        for index in range(0,len(commit.traces)):
            self.decodeTrace(decodedCommit, commit.traces[index])
        return decodedCommit

    def decodeTrace(self, commit, trace):
        type = struct.unpack('<B', trace[TYPE_RANGE])[0]
        if not (type ^ INS_MASK):
            self.decodeInstruction(commit, trace)
        if not (type ^ REG_MASK):
            self.decodeRegister(commit, trace)
        if not (type ^ MEM_MASK):
            self.decodeMemory(commit, trace)


    def decodeInstruction(self, commit, trace):
        instruction = bytes(trace[INSTRUCTION_RANGE])
        programCounter = None

        for index in range(0,len(commit.registers)):
            currentRegisterCommit = commit.registers[index]
            if currentRegisterCommit.register == Register.RIP:
                programCounter = currentRegisterCommit.value
                break

        if not instruction or not programCounter:
            return

        opcode = c_char_p(instruction)
        buffer = cast(create_string_buffer(INSTRUCTION_BUFFER_SIZE), POINTER(c_char))
        address = c_longlong(programCounter)
        size = c_int(INSTRUCTION_BUFFER_SIZE)

        libdecoder.decodeInstruction(opcode, address, buffer, size)
        description = cast(buffer,c_char_p).value.decode("utf-8")

        commit.instruction = InstructionCommit(description)

    def decodeRegister(self, commit, trace):
        register = struct.unpack('<B', trace[REGISTER_TYPE_RANGE])[0]
        value = struct.unpack('<Q', trace[REGISTER_VALUE_RANGE])[0]

        name = ""
        register = Register.getRegister(register)
        if register:
            name = register.name

        commit.registers.append(RegisterCommit(register,value))

    def decodeMemory(self, commit, trace):
        address = struct.unpack('<Q', trace[MEMORY_ADDRESS_RANGE])[0]
        value = struct.unpack('<Q', trace[MEMORY_VALUE_RANGE])[0]

        commit.memory.append(MemoryCommit(address,value))


def main():
    decoder = Decoder()
    trace = TraceHandler(TRACE_FILE_PATH)

    commits = trace.getCommits(20,1)

    for index in range(0,len(commits)):
        commit = decoder.decodeCommit(commits[index])
        commit.printCommit()

if __name__ == '__main__':
    main()
