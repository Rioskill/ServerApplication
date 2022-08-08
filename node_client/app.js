// const net = require('net');

// let socket = new net.Socket();
// socket.connect(8080, '127.0.0.1');

// socket.on('connect', ()=>{
//     console.log('connected');
// });

// socket.on('readable', ()=>{
//     const data = socket.read();

//     console.log(data.length);

//     const message_size = data.readInt32LE();
//     const message = data.toString('utf-8', 4);

//     console.log(data);
//     console.log(message_size + ' ' + message);
// });


// process.stdin.on('data', (message) => {
//     message = message.slice(0, -1);
//     const buffer = Buffer.alloc(4);
//     buffer.writeInt32LE(message.length);

//     socket.write(buffer);
//     socket.write(message);
// });

// const WebSocket = require('ws');

let WebSocketServer = new WebSocket("ws://127.0.0.1:8080");