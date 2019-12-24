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

DEFAULT_PAGE_SIZE = 10

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
            instruction = self.decodeInstruction(commit, trace)
            if instruction:
                commit.instruction = instruction

        if not (type ^ REG_MASK):
            register = self.decodeRegister(trace)
            if register:
                commit.registers.append(register)

        if not (type ^ MEM_MASK):
            memory = self.decodeMemory(trace)
            if memory:
                commit.memory.append(memory)


    def decodeInstruction(self, commit, trace):
        instruction = bytes(trace[INSTRUCTION_RANGE])
        programCounter = None

        for index in range(0,len(commit.registers)):
            currentRegisterCommit = commit.registers[index]
            if currentRegisterCommit.register == Register.RIP:
                programCounter = currentRegisterCommit.value
                break

        if not instruction or not programCounter:
            return None

        opcode = c_char_p(instruction)
        buffer = cast(create_string_buffer(INSTRUCTION_BUFFER_SIZE), POINTER(c_char))
        address = c_longlong(programCounter)
        size = c_int(INSTRUCTION_BUFFER_SIZE)

        libdecoder.decodeInstruction(opcode, address, buffer, size)
        description = cast(buffer,c_char_p).value.decode("utf-8")

        return InstructionCommit(description)

    def decodeRegister(self, trace):
        register = struct.unpack('<B', trace[REGISTER_TYPE_RANGE])[0]
        value = struct.unpack('<Q', trace[REGISTER_VALUE_RANGE])[0]

        name = ""
        register = Register.getRegister(register)

        return RegisterCommit(register,value)

    def decodeMemory(self, trace):
        address = struct.unpack('<Q', trace[MEMORY_ADDRESS_RANGE])[0]
        value = struct.unpack('<Q', trace[MEMORY_VALUE_RANGE])[0]

        return MemoryCommit(address,value)

    def getAllRegisters(self, index, tracer):
        registers = {}
        for register in Register:
            registers[register.name] = None
        registersLeft = len(registers)

        traces = tracer.getPreviousCommits(DEFAULT_PAGE_SIZE, index, True)[::-1]

        while registersLeft > 0:
            for i in range(0,len(traces)):
                commitRegisters = self.getAllTraceRegisters(traces[i])
                for commitRgisterName in commitRegisters:
                    if commitRgisterName in registers:
                        if registers[commitRgisterName] == None:
                            registers[commitRgisterName] = commitRegisters[commitRgisterName]
                            registersLeft -= 1
            if len(traces):
                if not tracer.hasMoreBefore(traces[-1].index):
                    break
            else:
                break

            index = traces[-1].index
            traces = tracer.getPreviousCommits(DEFAULT_PAGE_SIZE,index, False)[::-1]

        return registers

    def getAllTraceRegisters(self, commit):
        registers = {}
        traces = commit.traces
        if not traces:
            return {}

        for index in range(0,len(traces)):
            register = self.decodeRegister(traces[index])
            if register:
                if register.register:
                    registers[register.register.name] = register.value

        return registers

def main():
    decoder = Decoder()
    tracer = TraceHandler(TRACE_FILE_PATH)

    commits = tracer.getNextCommits(100,1)

    for index in range(0,len(commits)):
        commit = decoder.decodeCommit(commits[index])
        commit.printCommit()

    print("")
    registers = decoder.getAllRegisters(5, tracer)

    for register in registers:
        value = registers[register]
        if value == None:
            value = "NONE"
        else:
            value = hex(value)

        print(register + " -> " + value)

if __name__ == '__main__':
    main()
