#include "roomManager.hpp"

RoomManager::~RoomManager() {
    std::cout << "deleting rooms\n";
    for (auto it = rooms.begin(); it != rooms.end(); it++) {
        int id = it->first;
        delete rooms[id];
    }
}

Room *RoomManager::createRoom (Worker *worker) {
    int id = id_assigner.assignID();

    rooms.emplace(id, new Room(worker, id));

    return rooms[id];
}

void RoomManager::removeRoom (int id) {
    delete rooms[id];
    rooms.erase(id);

    id_assigner.releaseID(id);
}

void RoomManager::removeRoom (Room *room) {
    removeRoom(room->getManagerID());
}

Room *RoomManager::getRoom (int id) {
    return rooms[id];
}