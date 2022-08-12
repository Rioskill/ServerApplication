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

void setFileBody (HttpResponse &response, const std::string &file_name) {
    std::ifstream file_stream(file_name);
    std::stringstream buffer;
    buffer << file_stream.rdbuf();

    response.setBody(buffer.str());
}

HttpResponse root() {
    HttpResponse response;
    response.addHeader("Content-Type", "text/html");
    setFileBody(response, "pages/index.html");

    return response;
}

HttpResponse js() {
    HttpResponse response;
    response.addHeader("Content-Type", "application/javascript");
    setFileBody(response, "pages/main.js");

    return response;
}

HttpResponse defaultResponse() {
    HttpResponse response(404, "Not Found");
    return response;
}

std::tuple routes = {
    Route<"/">(root),
    Route<"/main.js">(js)
};

Router router(routes, defaultResponse);

HttpResponse check_path(const std::string &path) {
    return router.check_routes(path);
}

EventLoop loop;
WorkerManager workerManager(&loop);   // global so they properly delete everything after SIGINT
// RoomManager roomManager;
ClientManager clientManager(&loop, std::chrono::seconds(1));

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
    HttpProcessor processor(&loop, &clientManager, check_path);

    Acceptor acceptor(socket, &loop, &processor);

    acceptor.accept();

    loop.run();

    return 0;
}
