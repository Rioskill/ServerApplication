#include "roomManager.hpp"

int RoomManager::createRoom(Worker *worker) {
    int id = id_assigner.assignId();

    rooms.emplace(id, new Room(worker));

    return id;
}

void RoomManager::removeRoom(int id) {
    delete rooms[id];
    rooms.erase(id);
}