#include "connectionProcessor.hpp"

RoomEchoConnectionProcessor::RoomEchoConnectionProcessor (WorkerManager *workerManager, EventLoop *loop): workerManager(workerManager), loop(loop) {
    int worker_id = workerManager->createWorker();
    Worker *worker = workerManager->getWorker(worker_id);
    
    room = new Room(worker, worker_id);

    workerManager->startWorker(worker_id);
}

RoomEchoConnectionProcessor::~RoomEchoConnectionProcessor() {
    workerManager->stopWorker(room->getWorkerId());
    workerManager->removeWorker(room->getWorkerId());

    delete room;
}

void RoomEchoConnectionProcessor::close_connection (Client *client, int client_id) {
    delete client;
    room->removeClient(client_id);
}

void RoomEchoConnectionProcessor::process_message (Client *client, int client_id) {
    client->read([this, client, client_id](int size, char *data){
        std::string res(data, size);
        std::cout << "recieved \"" << res << "\" from client"<< std::endl;

        if (res == "exit") {
            close_connection(client, client_id);
        } else {
            Worker *worker = room->getWorker();

            worker->write(size, data, [this, client_id, worker, client]() {
                worker->read([this, client_id, client](int response_size, char *data){
                    room->broadcast(response_size, data, [this, client, client_id]() {

                        process_message(client, client_id);
                    });
                });
            });
        }
    });
}

void RoomEchoConnectionProcessor::process (int client_fd) {
    Client *client = new Client(client_fd, loop);

    int client_id = room->addClient(client);
    process_message(client, client_id);
}