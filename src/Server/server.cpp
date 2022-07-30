#include <stdio.h>
#include <cstring>
#include <functional>
#include <string>
#include <iostream>
#include <queue>
#include <errno.h>
#include <deque>
#include <unordered_map>

#include "eventLoop.hpp"
#include "acceptor.hpp"

#include "worker.hpp"

void clientAcceptor() {
    const int port = 8080;
    const char* ip = "127.0.0.1";

    NetworkSocket socket (ip, port);

    EventLoop loop;

    Acceptor acceptor(&loop, socket);

    acceptor.accept();

    loop.run();
}

//TODO: close pipes;

int main () {

    const int port = 8080;
    const char* ip = "127.0.0.1";

    NetworkSocket socket (ip, port);

    EventLoop loop;

    Worker worker("fromWorker", "toWorker", &loop);

    WorkerEchoAcceptor acceptor(&loop, socket, &worker);

    // Acceptor acceptor(&loop, socket);

    acceptor.accept();

    // worker.read();

    // int size = 4;
    // char *buffer = "smth";

    // worker.write(size, buffer, [&worker](){
    //     worker.read();
    // });

    loop.run();

    return 0;
}
