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

#include "worker.hpp"

EventLoop loop;
//Worker worker("from", "to", &loop);   // worker is global so it is deleted after SIGINT
WorkerManager manager(&loop);

void SIGINT_handler(int signum) {
    loop.stop();

    exit(0);
}

int main () {
    std::signal(SIGINT, SIGINT_handler);


    const int port = 8080;
    const char* ip = "127.0.0.1";

    NetworkSocket socket (ip, port);

    int worker_id = manager.createWorker();
    manager.startWorker(worker_id);

    WorkerEchoAcceptor acceptor(&loop, socket, &manager);

    acceptor.accept();

    loop.run();

    return 0;
}
