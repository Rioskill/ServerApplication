#include <csignal>
#include <chrono>
#include <tuple>

#include "eventLoop.hpp"
#include "acceptor.hpp"
#include "workerManager.hpp"
#include "roomManager.hpp"
#include "connectionProcessor.hpp"
#include "clientManager.hpp"
#include "router.hpp"

EventLoop loop;
WorkerManager workerManager(&loop);   // global so they properly delete everything after SIGINT
// RoomManager roomManager;
ClientManager clientManager(&loop, std::chrono::seconds(2));

void SIGINT_handler(int signum) {
    loop.stop();

    exit(0);
}

int main () {
    std::signal(SIGINT, SIGINT_handler);

    const int port = 8080;
    const char* ip = "127.0.0.1";

    NetworkSocket socket (ip, port);

    // RoomEchoConnectionProcessor processor(&workerManager, &roomManager, &loop);
    // BasicEchoConnectionProcessor processor(&loop);
    HttpProcessor processor(&loop, &clientManager);

    Acceptor acceptor(socket, &loop, &processor);

    acceptor.accept();

    loop.run();

    return 0;
}
