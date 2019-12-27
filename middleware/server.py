from flask import Flask, render_template
from flask_socketio import SocketIO

from trace_handler import *
from decoder import *

import json

TRACE_FILE_PATH = "../traces/trace"

server = Flask(__name__, static_folder="../frontend/dist", template_folder="../frontend")
socketio = SocketIO(server)

decoder = Decoder()
tracer = TraceHandler(TRACE_FILE_PATH)

def parseJSON(response):
    data = str(response).replace("\'", "\"")
    return json.loads(data)

def validate(json, keys):
    for index in range(0,len(keys)):
        if not keys[index] in json:
            return False
        elif json[keys[index]] == None:
            return False
    return True

@server.route("/")
def index():
    return render_template("index.html")

@socketio.on("hasMore")
def hasMore(request):
    data = parseJSON(request)

    if not validate(data, ["index", "type"]):
        response = { 'error': 'missing values' }
        socketio.emit('hasMore', str(response))
        return

    index = data["index"]
    type = data["type"]

    hasMore = False

    if type == "after":
        hasMore = tracer.hasMoreAfter(index)
    elif type == "before":
        hasMore = tracer.hasMoreBefore(index)
    else:
        response = { 'error': 'unknown type' }
        socketio.emit('hasMore', str(response))
        return

    response = { "hasMore": hasMore }
    socketio.emit("hasMore", json.dumps(response))

@socketio.on('memoryDump')
def getMemory(request):
    memory, lastIndex = decoder.getMemoryDump(tracer)
    memoryArray = {}

    for address in memory:
        key = hex(address)
        commits = memory[address]
        memoryArray[key] = []
        for index in range(0, len(commits)):
            memoryArray[key].append(commits[index].toDictionary())

    response = { "memory": memoryArray, 'lastIndex': lastIndex }
    socketio.emit('commits', json.dumps(response))

@socketio.on('registers')
def getRegisters(request):
    data = parseJSON(request)

    if not validate(data, ["index"]):
        response = { 'error': 'missing values' }
        socketio.emit('registers', str(response))
        return

    index = data["index"]

    registersArray = {}
    registers = decoder.getAllRegisters(index, tracer)
    for name in registers:
        value = registers[name]
        if value != None:
            registersArray[name] = hex(value)
        else:
            registersArray[name] = None

    response = { "registers": registersArray }
    socketio.emit('registers', json.dumps(response))

@socketio.on('commits')
def getCommits(request):
    data = parseJSON(request)
    if not validate(data, ["index", "amount"]):
        response = { 'error': 'missing values' }
        socketio.emit('commits', str(response))
        return

    index = data["index"]
    amount = data["amount"]

    fetchNext = True
    if "direction" in data:
        if data["direction"] == "previous":
            fetchNext = False

    includeIndex = True
    if "includeIndex" in data:
        if data["includeIndex"] == False:
            includeIndex = False

    commits = []
    if fetchNext:
        commits = tracer.getNextCommits(amount,index,includeIndex)
    else:
        commits = tracer.getPreviousCommits(amount,index,includeIndex)

    commitsArray = []
    for index in range(0,len(commits)):
        commit = decoder.decodeCommit(commits[index])
        commit.printCommit()
        commitsArray.append(commit.toDictionary())

    hasMore = False
    if len(commits):
        if fetchNext:
            index = commits[-1].index
            if index:
                hasMore = tracer.hasMoreAfter(index)
        else:
            index = commits[0].index
            if index:
                hasMore = tracer.hasMoreBefore(index)

    response = { "commits": commitsArray, 'hasMore': hasMore }
    socketio.emit('commits', json.dumps(response))



if __name__ == "__main__":
    server.run()
