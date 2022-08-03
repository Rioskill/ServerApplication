#ifndef ROOM
#define ROOM

#include <map>

#include "worker.hpp"
#include "client.hpp"
#include "idAssigner.hpp"

class Room {
private:
    Worker *worker;
    std::map<int, Client*> clients;
    UniqueIDAssigner id_assigner;

    int managerID;
public:
    Room (Worker *worker, int managerID = -1): worker(worker), managerID(managerID) {}

    void setManagerID (int managerID) {
        this->managerID = managerID;
    }

    int getManagerID() {
        return managerID;
    }
    
    Worker *getWorker() {
        return worker;
    }

    int getWorkerManagerId() {
        return worker->getManagerID();
    }

    int addClient (Client *client) {
        int id = id_assigner.assignID();
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

    int clientsCount() {
        return clients.size();
    }

    bool empty() {
        return clients.empty();
    }
};


#endif