#ifndef ROOM
#define ROOM

#include <map>

#include "worker.hpp"
#include "client.hpp"

class Room {
private:
    Worker *worker;
    std::map<int, Client*> clients;

    unsigned int counter;

public:
    Room (Worker *worker): worker(worker){
        counter = 0;
    }
    
    Worker *getWorker() {
        return worker;
    }

    int getWorkerManagerId() {
        return worker->getManagerID();
    }

    int addClient (Client *client) {
        int id = counter++;
        clients.emplace(id, client);
        return id;
    }

    void removeClient (int id) {
        clients.erase(id);
    }

    void removeClient (Client *client) {
        removeClient(client->getManagerID());
    }

    void broadcast (unsigned int bytes, void *message, std::function<void()> cb) {
        for (auto &pair: clients) {
            Client *client = pair.second;
            client->write(bytes, message, cb);
        }
    }
};


#endif