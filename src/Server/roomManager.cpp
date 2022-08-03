#include "roomManager.hpp"

int RoomManager::createRoom(Worker *worker, int worker_id) {
    int id = id_assigner.assignId();

    rooms.emplace(id, new Room(worker, worker_id));

    return id;
}

void RoomManager::removeRoom(int id) {
    delete rooms[id];
    rooms.erase(id);
}