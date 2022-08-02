#ifndef ROOM
#define ROOM

#include <vector>

#include "worker.hpp"
#include "client.hpp"

class Room {
private:
    Worker *worker;
    int worker_id;
    std::vector<Client*> clients;

public:
    Room (Worker *worker, int worker_id): worker(worker), worker_id(worker_id) {}
    
    Worker *getWorker() {
        return worker;
    }

    int getWorkerId() {
        return worker_id;
    }

    void addClient (Client *client) {
        clients.push_back(client);
    }

    void broadcast (unsigned int bytes, void *message, std::function<void()> cb) {
        for (Client *client: clients) {
            client->write(bytes, message, cb);
        }
    }
};


#endif