#include "connectionProcessor.hpp"

RoomEchoConnectionProcessor::RoomEchoConnectionProcessor (WorkerManager *workerManager, RoomManager *roomManager, EventLoop *loop): workerManager(workerManager), roomManager(roomManager), loop(loop) {}

RoomEchoConnectionProcessor::~RoomEchoConnectionProcessor() {}

std::string RoomEchoConnectionProcessor::get_room_name (int room_id) {
    for (auto it = room_ids.begin(); it != room_ids.end(); it++) {
        if (it->second == room_id)
            return it->first;
    }
    throw std::exception();
}

void RoomEchoConnectionProcessor::close_connection (Room *room, Client *client) {
    room->removeClient(client);
    delete client;

    if (room->empty()) {
        int room_id = room->getManagerID();
        std::string room_name = get_room_name(room_id);
        room_ids.erase(room_name);
        workerManager->removeWorker(room->getWorkerManagerId());
        roomManager->removeRoom(room_id);
        std::cout << "deleted room with name: " << std::quoted(room_name) << ", with id: " << room_id << std::endl;
    } 
}

void RoomEchoConnectionProcessor::process_message (Room *room, Client *client) {
    client->read([this, client, room](int size, char *data){
        std::string res(data, size);
        std::cout << "recieved \"" << res << "\" from client"<< std::endl;

        if (res == "exit") {
            close_connection(room, client);
        } else {
            Worker *worker = room->getWorker();

            worker->write(size, data, [this, worker, room, client]() {
                worker->read([this, room, client](int response_size, char *data){
                    room->broadcast(response_size, data, [this, room, client]() {
                        process_message(room, client);
                    });
                });
            });
        }
    });
}

Room *RoomEchoConnectionProcessor::connect_to_room(const std::string &room_name) {
    std::cout << "trying to connect to room with name: " << room_name << std::endl;

    auto room_id_pair = room_ids.find(room_name);

    Room *room;

    if (room_id_pair == room_ids.end()) {
        Worker *worker = workerManager->createWorker();
        workerManager->startWorker(worker);
        room = roomManager->createRoom(worker);
    } else {
        room = roomManager->getRoom(room_id_pair->second);
    }

    room_ids.emplace(room_name, room->getManagerID());

    std::cout << "connected to room with id: " << room->getManagerID() << std::endl;

    return room;
}

void RoomEchoConnectionProcessor::process (Client *client) {
    client->read([this, client](int size, char *data){
        std::string room_name(data, size);

        Room *room = connect_to_room(room_name);

        int client_id = room->addClient(client);
        client->setManagerID(client_id);

        process_message(room, client);
    });
}

void BasicEchoConnectionProcessor::process (Client *client) {
    client->read([this, client](int size, char *data){
        std::string message(data, size);
        std::cout << "recieved: " << std::quoted(message) << std::endl;

        client->write(size, data, [client](){
            client->close();
            delete client;
        });
    });
}

void HttpProcessor::setFileBody (HttpResponse &response, const std::string &file_name) {
    std::ifstream file_stream(file_name);
    std::stringstream buffer;
    buffer << file_stream.rdbuf();

    response.setBody(buffer.str());
}

void HttpProcessor::respond (Client *client, const HttpRequest &request) {
    HttpResponse response(200, "OK");

    if (request.path == "/") {
        response.addHeader("Content-Type", "text/html");
        setFileBody(response, "pages/index.html");
    }
    else if (request.path == "/main.js") {
        response.addHeader("Content-Type", "application/javascript");
        setFileBody(response, "pages/main.js");
    } else {
        response.setStatus(404, "Not Found");
    }

    response.addHeader("Connection", "keep-alive");

    std::string response_message = response.render();

    bool keep_alive = true;

    if (request.getHeaders().contains("Connection")) {
        if (*request.getHeaders().at("Connection").begin() == "keep-alive")
            keep_alive = true;
        else
            keep_alive = false;
    }

    // keep_alive = false;

    client->write(response_message.size(), response_message.c_str(), [this, client, keep_alive](){
        clientManager->updateTimeout(client);
        process(client);
    });
}

void HttpProcessor::process (Client *client) {

    if (client->getManagerID() == -1) {
        clientManager->addClient(client);
        clientManager->updateTimeout(client);
        clientManager->scheduleCheckTimeout(client);
    }

    client->read([this, client](int size, char *data){
        std::string message(data, size);
        // std::cout << "recieved: " << std::quoted(message) << std::endl << std::endl;

        HttpRequest request(message);

        if (request.getHeaders().contains("Content-Length")) {
            unsigned int content_length = std::stoi(*request.getHeaders().at("Content-Length").begin());

            client->read_n_bytes(content_length, [this, client, request](int size, char *data){
                std::string body_string(data, size);
                std::cout << "body: " << body_string << std::endl << std::endl;
                respond(client, request);
            });
        } else {
            respond(client, request);
        }
    });
}