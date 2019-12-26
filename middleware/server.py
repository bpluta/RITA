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
    
@socketio.on('commits')
def getCommits(request):
    data = parseJSON(request)
    if not validate(data, ["index", "amount"]):
        response = { 'error': 'missing values' }
        socketio.emit('commits', str(response))

    index = data["index"]
    amount = data["amount"]

    commits = tracer.getNextCommits(amount,index)

    commitsArray = []
    for index in range(0,len(commits)):
        commit = decoder.decodeCommit(commits[index])
        commitsArray.append(commit.toDictionary())

    hasMore = False
    if len(commits):
        index = commits[-1].index
        if index:
            hasMore = tracer.hasMoreAfter(index)

    response = { "commits": commitsArray, 'hasMore': hasMore }
    socketio.emit('commits', json.dumps(response))

if __name__ == "__main__":
    server.run()
