import React from "react";
import openSocket from "socket.io-client";

const socket = openSocket("http://localhost:5000/");

var next = false

const sendMessage = () => {
    let request = {
        index: 10,
        amount: 5,
        direction: next ? "next" : "previous",
        includeIndex: false,
    }
    console.log(request)
    socket.emit("commits", JSON.stringify(request));
}


export default class App extends React.Component {

    constructor(props) {
        super(props)
        socket.on("commits", message => {
            let data = JSON.parse(message)
            next = !next
            console.log(data)
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
