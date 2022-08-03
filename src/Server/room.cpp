#include "room.hpp"

Room::Room (Worker *worker, int managerID = -1): worker(worker), managerID(managerID) {}

void Room::setManagerID (int managerID) {
    this->managerID = managerID;
}

int Room::getManagerID() {
    return managerID;
}

Worker *Room::getWorker() {
    return worker;
}

int Room::getWorkerManagerId() {
    return worker->getManagerID();
}

int Room::addClient (Client *client) {
    int id = id_assigner.assignID();
    clients.emplace(id, client);
    return id;
}

void Room::removeClient (int id) {
    clients.erase(id);
}

void Room::removeClient (Client *client) {
    removeClient(client->getManagerID());
}

void Room::broadcast (unsigned int bytes, void *message, std::function<void()> cb) {
    for (auto &pair: clients) {
        Client *client = pair.second;
        client->write(bytes, message, cb);
    }
}

int Room::clientsCount() {
    return clients.size();
}

bool Room::empty() {
    return clients.empty();
}