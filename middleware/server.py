from flask import Flask, render_template
from flask_socketio import SocketIO
import json

app = Flask(__name__, static_folder="../frontend/dist", template_folder="../frontend")
socketio = SocketIO(app)

def parseJSON(response):
    data = str(response).replace("\'", "\"")
    return json.loads(data)

@app.route("/")
def index():
    return render_template("index.html")

@socketio.on('event')
def handleEvent(data):
    print(str(data))
    json = parseJSON(data)
    message = json.get("message", "None")
    response = {'message': message}
    socketio.emit('event', str(response))

if __name__ == "__main__":
    app.run()
