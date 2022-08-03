#include <stdio.h>
#include <cstring>
#include <functional>
#include <string>
#include <iostream>
#include <queue>
#include <errno.h>
#include <deque>
#include <unordered_map>
#include <csignal>

#include "eventLoop.hpp"
#include "acceptor.hpp"
#include "workerManager.hpp"
#include "roomManager.hpp"
#include "connectionProcessor.hpp"

EventLoop loop;
WorkerManager workerManager(&loop);   // global so they properly delete everything after SIGINT
RoomManager roomManager;

void SIGINT_handler(int signum) {
    loop.stop();

    exit(0);
}

int main () {
    std::signal(SIGINT, SIGINT_handler);


    const int port = 8080;
    const char* ip = "127.0.0.1";

    NetworkSocket socket (ip, port);

    RoomEchoConnectionProcessor processor(&workerManager, &roomManager, &loop);
    // BasicEchoConnectionProcessor processor(&loop);

    Acceptor acceptor(socket, &loop, &processor);

    acceptor.accept();

    loop.run();

    return 0;
}
