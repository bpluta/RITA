import os
import math
import binascii

TRACE_FILE_PATH = "../traces/trace"
COMMIT_LINE_SIZE = 21

class Trace:
    def __init__(self, index):
        self.index = index
        self.traces = []

    def addTrace(self, bytes):
        self.traces.append(bytes)

    def isEmpty(self):
        return self.traces

class TraceHandler:
    def __init__(self, path):
        self.path = path
        self.open()

    def open(self):
        self.fd = open(self.path, "rb")

    def close(self):
        self.fd.close()

    def getSize(self):
        return os.path.getsize(self.path)

    def getCommits(self, amount, fromIndex):
        commits = []
        position = self.findCommit(fromIndex)
        commit = Trace(fromIndex)

        while position != None:
            currentIndex = self.getIndexOfLine(position)
            currentIndex, trace = self.readTrace(position)

            if currentIndex == None or commit == None:
                break
            if currentIndex > fromIndex + amount:
                break

            if currentIndex > commit.index:
                commits.append(commit)
                commit = Trace(currentIndex)

            commit.addTrace(trace)

            position = self.getNextLine(position)

        if commit.isEmpty == False:
            commits.append(commit)

        return commits

    def findCommit(self, index):
        lower = 0
        upper = self.getLastLineOffset()
        middle = self.getMiddleLine(lower, upper)

        lowerIndex = self.getIndexOfLine(lower)
        middleIndex = self.getIndexOfLine(middle)
        upperIndex = self.getIndexOfLine(upper)

        while True:
            if (index < lowerIndex) or (index > upperIndex):
                return None

            if lowerIndex == index:
                return self.getCommit(lower)
            if upperIndex == index:
                return self.getCommit(upper)
            if middleIndex == index:
                return self.getCommit(middle)

            if index > middleIndex:
                lower = middle
                middle = self.getMiddleLine(lower, upper)

                lowerIndex = self.getIndexOfLine(lower)
                middleIndex = self.getIndexOfLine(middle)
            else:
                upper = middle
                middle = self.getMiddleLine(lower, upper)

                upperIndex = self.getIndexOfLine(upper)
                middleIndex = self.getIndexOfLine(middle)

        return None

    def getIndexOfLine(self, offset):
        initialPosition = self.fd.tell()

        position = self.alignToBeginOfCommmitLine(offset)
        self.fd.seek(position)
        index = self.readInt(4)

        self.fd.seek(initialPosition)
        return index

    def getCommit(self, offset):
        initialPosition = self.fd.tell()

        position = self.alignToBeginOfCommmitLine(offset)
        previousPosition = position

        self.fd.seek(position)

        index = self.readInt(4)
        currentIndex = index

        while True:
            self.fd.seek(previousPosition)
            tmpIndex = self.readInt(4)

            if tmpIndex != index:
                break

            position = previousPosition

            previousPosition = self.getPreviousLine(position)
            if previousPosition == None:
                break

        self.fd.seek(initialPosition)
        return position

    def getNextLine(self, offset):
        size = self.getSize()
        newLine = self.alignToBeginOfCommmitLine(offset)
        lastLine = self.getLastLineOffset()

        if newLine == lastLine:
            return None

        newLine += COMMIT_LINE_SIZE
        return newLine

    def getPreviousLine(self, offset):
        if offset < COMMIT_LINE_SIZE:
            return None
        line = offset - COMMIT_LINE_SIZE
        return self.alignToBeginOfCommmitLine(line)

    def readInt(self, size):
        return int.from_bytes(self.fd.read(size), "little")

    def readTrace(self, offset):
        self.fd.seek(offset)
        index = self.readInt(4)
        data = bytearray(self.fd.read(COMMIT_LINE_SIZE))
        return index, data

    def getLastLineOffset(self):
        size = self.getSize()
        line = size - COMMIT_LINE_SIZE
        return self.alignToBeginOfCommmitLine(line)

    def getMiddleLine(self, lower, upper):
        middle = lower + int(math.floor((upper-lower)/2))
        return self.alignToBeginOfCommmitLine(middle)

    def alignToBeginOfCommmitLine(self, position):
        return position - (position % COMMIT_LINE_SIZE)

def main():
    trace = TraceHandler(TRACE_FILE_PATH)

    for index in range(0,50):
        commit = trace.findCommit(index)
        if commit != None:
            print("commit " + str(index) + " :: line " + str(int(commit/COMMIT_LINE_SIZE)))

    print("\nCommits\n")
    commits = trace.getCommits(5,16)
    for index in range(0,len(commits)):
        print("index : " + str(commits[index].index) + " :::: amount : " + str(len(commits[index].traces)))

if __name__ == '__main__':
    main()
