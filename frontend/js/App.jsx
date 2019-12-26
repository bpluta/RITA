import React from "react";
import openSocket from "socket.io-client";

const socket = openSocket("http://localhost:5000/");

const sendMessage = () => {
    let response = {
        message: "Hello World!"
    }
    socket.emit("event", JSON.stringify(response));
}

export default class App extends React.Component {

    constructor(props) {
        super(props)
        socket.on("event", message => {
            console.log(message)
        })
    }

    render() {
        return (
            <button onClick={sendMessage}>
                Message
            </button>
        )
    }
}
