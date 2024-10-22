import os
import math
import binascii

TRACE_FILE_PATH = "../traces/trace"
COMMIT_LINE_SIZE = 21

class Trace:
    def __init__(self, index):
        self.index = index
        self.traces = []

    def appendTrace(self, bytes):
        self.traces.append(bytes)

    def prependTrace(self, bytes):
        self.traces.insert(0, bytes)

    def isEmpty(self):
        if self.traces:
            return len(self.traces) == 0
        return True

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

    def getPreviousCommits(self, amount, fromIndex, includeBound = True):
        offset = 0
        if not includeBound:
            offset = 1

        initialIndex = fromIndex - offset

        lastIndex = self.getIndexOfLine(self.getLastLineOffset())
        if initialIndex > lastIndex:
            initialIndex = lastIndex

        commits = []
        position = self.getLastLineOfCommit(self.findCommit(initialIndex))
        commit = Trace(initialIndex)

        while position != None:
            currentIndex = self.getIndexOfLine(position)
            currentIndex, trace = self.readTrace(position)

            if currentIndex == None or commit == None:
                break
            if currentIndex <= initialIndex - amount:
                break

            if currentIndex < commit.index:
                commits.append(commit)
                commit = Trace(currentIndex)

            commit.prependTrace(trace)

            position = self.getPreviousLine(position)

        if commit.isEmpty() == False:
            commits.append(commit)

        return commits[::-1]


    def getNextCommits(self, amount, fromIndex, includeBound = True):
        offset = 0
        if not includeBound:
            offset = 1

        initialIndex = fromIndex + offset
        if initialIndex < 1:
            initialIndex = 1

        commits = []
        position = self.findCommit(initialIndex)
        commit = Trace(initialIndex)

        while position != None:
            currentIndex = self.getIndexOfLine(position)
            currentIndex, trace = self.readTrace(position)

            if currentIndex == None or commit == None:
                break
            if currentIndex >= initialIndex + amount:
                break

            if currentIndex > commit.index:
                commits.append(commit)
                commit = Trace(currentIndex)

            commit.appendTrace(trace)

            position = self.getNextLine(position)

        if commit.isEmpty() == False:
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

    def getLastLineOfCommit(self, offset):
        position = self.alignToBeginOfCommmitLine(offset)
        initialIndex = self.getIndexOfLine(position)
        currentPosition = position
        while currentPosition:
            index = self.getIndexOfLine(currentPosition)
            if index != initialIndex:
                break
            position = currentPosition
            currentPosition = self.getNextLine(currentPosition)
        return position

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

    def hasMoreBefore(self, index):
        firstIndex = self.getIndexOfLine(0)
        if firstIndex:
            if index > firstIndex:
                return True
        return False

    def hasMoreAfter(self, index):
        lastIndex = self.getIndexOfLine(self.getLastLineOffset())
        if lastIndex:
            if index < lastIndex:
                return True
        return False

def main():
    trace = TraceHandler(TRACE_FILE_PATH)

    print("\nCommits\n")
    commits = trace.getNextCommits(3,8)
    for index in range(0,len(commits)):
        print("index : " + str(commits[index].index) + " :::: amount : " + str(len(commits[index].traces)))

    print("")

    commits = trace.getPreviousCommits(3,10,True)
    for index in range(0,len(commits)):
        print("index : " + str(commits[index].index) + " :::: amount : " + str(len(commits[index].traces)))

if __name__ == '__main__':
    main()
