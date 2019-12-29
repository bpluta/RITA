import openSocket from "socket.io-client";

const socket = openSocket("http://localhost:5000/");

export default class Api {
    static registerCommitHandler(callback) {
        console.log("register handler")
        socket.on('commits', callback)
    }

    static unregisterCommitHandler() {
        socket.off('commits')
    }

    static registerMemoryDumpHandler(callback) {
        socket.on('memoryDump', callback)
    }

    static unregisterMemoryDumpHandler() {
        socket.off('memoryDump')
    }

    static registerRegisterHandler(callback) {
        socket.on('registers', callback)
    }

    static unregisterRegisterHandler() {
        socket.off('registers')
    }

    static registerHasMoreHandler(callback) {
        socket.on('hasMore', callback)
    }

    static unegisterHasMoreHandler() {
        socket.off('hasMore')
    }

    static getCommits(index, amount, direction, includeIndex) {
        let request = {
            index: index,
            amount: amount,
            direction: direction,
            includeIndex: includeIndex,
        }
        console.log(request)
        return Api.emit("commits", JSON.stringify(request))
    }

    static getMemoryDump() {
        return Api.emit("memoryDump", "")
    }

    static getRegisters(index) {
        let request = {
            index: index
        }
        console.log(request)
        return Api.emit("registers", JSON.stringify(request))
    }

    static hasMoreCommits(index, type) {
        let request = {
            index: index,
            type: type,
        }
        console.log(request)
        return Api.emit("hasMore", JSON.stringify(request))
    }

    static emit(event, data) {
        return new Promise((resolve, reject) => {
            if (!socket) {
                reject('No socket connection.');
            } else {
                socket.emit(event, data, (response) => {
                    if (response.error) {
                        console.error(response.error);
                        reject(response.error);
                    } else {
                        resolve();
                    }
                });
            }
        });
    }
}
