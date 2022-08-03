#include "connectionProcessor.hpp"

RoomEchoConnectionProcessor::RoomEchoConnectionProcessor (WorkerManager *workerManager, EventLoop *loop): workerManager(workerManager), loop(loop) {
    Worker *worker = workerManager->createWorker();
    
    room = new Room(worker);
    workerManager->startWorker(worker);
}

RoomEchoConnectionProcessor::~RoomEchoConnectionProcessor() {
    workerManager->stopWorker(room->getWorker());
    workerManager->removeWorker(room->getWorker());

    delete room;
}

void RoomEchoConnectionProcessor::close_connection (Client *client) {
    room->removeClient(client);
    delete client;
}

void RoomEchoConnectionProcessor::process_message (Client *client) {
    client->read([this, client](int size, char *data){
        std::string res(data, size);
        std::cout << "recieved \"" << res << "\" from client"<< std::endl;

        if (res == "exit") {
            close_connection(client);
        } else {
            Worker *worker = room->getWorker();

            worker->write(size, data, [this, worker, client]() {
                worker->read([this, client](int response_size, char *data){
                    room->broadcast(response_size, data, [this, client]() {

                        process_message(client);
                    });
                });
            });
        }
    });
}

void RoomEchoConnectionProcessor::process (int client_fd) {
    Client *client = new Client(client_fd, loop);

    int client_id = room->addClient(client);
    client->setManagerID(client_id);

    process_message(client);
}