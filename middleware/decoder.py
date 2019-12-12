from ctypes import *
libdecoder = CDLL("./lib/build/libdecoder.dylib")

INS_MASK = 0b0001
REG_MASK = 0b0010
MEM_MASK = 0b0100

INSTRUCTION_BUFFER_SIZE = 256

TRACE_FILE_PATH = "../traces/trace"

class Commit:
    def __init__(self, destination, value):
        self.destination = destination
        self.value = value

class Instruction:
    def __init__(self, id):
        self.id = id
        self.address = None
        self.opcode = None
        self.description = None
        self.commits = []

    def decodeInstruction(self):
        if not self.opcode or not self.address:
            return None
        opcode = c_char_p(self.opcode)
        buffer = cast(create_string_buffer(INSTRUCTION_BUFFER_SIZE), POINTER(c_char))
        address = c_longlong(self.address)
        size = c_int(INSTRUCTION_BUFFER_SIZE)

        libdecoder.decodeInstruction(opcode, address, buffer, size)
        self.description = cast(buffer,c_char_p).value.decode("utf-8")
        return self.description

class Decoder:
    def __init__(self, filename):
        self.filename = filename
        self.previous = []
        self.currentInstruction = None
        libdecoder.init();

    def open(self):
        self.inputFile = open(self.filename,"rb")
        self.currentPosition = 0

    def nextInstruction(self):
        id = int.from_bytes(self.inputFile.read(4), "little")
        type = int.from_bytes(self.inputFile.read(1), "little")

        if not (type ^ INS_MASK):
            if not self.currentInstruction:
                self.currentInstruction = Instruction(id)
            if self.currentInstruction.id != id:
                # if self.currentInstruction:
                #     self.previous.append(self.currentInstruction)
                self.currentInstruction = Instruction(id)
            self.currentInstruction.opcode = bytes(bytearray(self.inputFile.read(15)))
            self.currentInstruction.address = 0x007FFFFFFF400000
            blank = self.inputFile.read(1)
            self.currentInstruction.decodeInstruction()
            return True
        return False

    def close(self):
        self.inputFile.close()

decoder = Decoder(TRACE_FILE_PATH)
decoder.open()

while decoder.nextInstruction():
    if decoder.currentInstruction:
        print(str(decoder.currentInstruction.id) + " :: " + decoder.currentInstruction.description)

decoder.close()
