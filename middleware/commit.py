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

    def toDictionary(self):
        registers = []
        for index in range (0,len(self.registers)):
            registers.append(self.registers[index].toDictionary())

        memory = []
        for index in range (0,len(self.memory)):
            registers.append(self.memory[index].toDictionary())

        return {
            "index": self.index,
            "registers": registers,
            "memory": memory,
            "instruction": self.instruction.toDictionary(),
        }

class RegisterCommit:
    def __init__(self, register, value):
        self.register = register
        self.value = value

    def toDictionary(self):
        return {
            "register": self.register.name,
            "value": hex(self.value)
        }

class MemoryCommit:
    def __init__(self, address, value):
        self.address = address
        self.value = value

    def toDictionary(self):
        return {
            "address": hex(self.address),
            "value": hex(self.value),
        }

class InstructionCommit:
    def __init__(self, value):
        self.value = value

    def toDictionary(self):
        return {
            "value": self.value
        }

class CommitData:
    def __init__(self, index, value):
        self.index = index
        self.value = value

    def toDictionary(self):
        return {
            "index": self.index,
            "value": hex(self.value)
        }
