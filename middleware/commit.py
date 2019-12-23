from register import *

class Commit:
    def __init__(self, index):
        self.index = index
        self.registers = []
        self.memory = []
        self.instruction = None

    def printCommit(self):
        print("Commit #" + str(self.index))
        for index in range(0,len(self.registers)):
            item = self.registers[index]
            print("Register :: " + item.register.name + " -> " + str(hex(item.value)))

        for index in range(0,len(self.memory)):
            item = self.memory[index]
            print("Memory :: " + hex(item.address) + " -> " + str(hex(item.value)))

        print("Instruction :: " + self.instruction.value)

class RegisterCommit:
    def __init__(self, register, value):
        self.register = register
        self.value = value

class MemoryCommit:
    def __init__(self, address, value):
        self.address = address
        self.value = value

class InstructionCommit:
    def __init__(self, value):
        self.value = value
