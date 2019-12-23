from enum import Enum

class Register(Enum):
    RAX = 1
    RBX = 2
    RCX = 3
    RDX = 4
    RSP = 5
    RBP = 6
    RSI = 7
    RDI = 8
    R8 = 9
    R9 = 10
    R10 = 11
    R11 = 12
    R12 = 13
    R13 = 14
    R14 = 15
    R15 = 16
    RIP = 17
    RFLAGS = 18
    CS = 19
    FS = 20
    GS = 21

    @classmethod
    def hasValue(cls, value):
        return value in cls._value2member_map_

    @classmethod
    def getRegister(cls, type):
        if Register.hasValue(type):
            return Register(type)
        return None
