const express = require('express');
const http = require('http');
const fs = require('fs');

const app = express();
const server = http.createServer(app);
const socketio = require("socket.io");

const TAIL_LOG_FILE = "../tail_log_file.txt"


const userSockets = new Map();

const io = socketio(server,{
	cors:{
		origin:"*",
		methods:["GET", "POST"]
	}
} );



let logs = fs.readFileSync(TAIL_LOG_FILE, "utf8");


fs.watchFile(TAIL_LOG_FILE,{persistent: true, interval: 500}, (cur, prev) => {
	console.log("tail_logs changed: ", new Date());
	fs.readFile(TAIL_LOG_FILE, "utf8", (err, data) => {
		if(err){
			console.log("Error reading file: ", err);
			return;
		}
		io.emit("log_update", data);
	})
});

io.on('connection', (socket) => {
	userSockets[socket.id] = socket;
  console.log('a user connected');
	socket.emit('intial_logs', logs);
	socket.on('disconnect', () => {
		userSockets.delete(socket.id);	
	})
});

server.listen(3000, () => {
  console.log('listening on *:3000');
});