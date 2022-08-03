const net = require('net');
const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

let socket = new net.Socket();
socket.connect(8080, '127.0.0.1');

socket.on('connect', ()=>{
    console.log('connected');

    read_and_write();
});

socket.on('data', (data)=>{
    console.log(data);
    const size = data.readInt32LE();
    const message = data.toString('utf-8', 4);

    console.log('recieved: "' + message + '"');
});

function read_and_write () {
    rl.on("line", (message)=>{
        const message_size = message.length;
    
        const buf = Buffer.alloc(4);
        buf.writeInt32LE(message_size);
    
        console.log(buf);
    
        socket.write(buf);
    
        socket.write(message);

        read_and_write();
    });
}