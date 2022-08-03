#include "roomManager.hpp"

int RoomManager::createRoom(Worker *worker) {
    int id = id_assigner.assignID();

    rooms.emplace(id, new Room(worker, id));

    return id;
}

void RoomManager::removeRoom(int id) {
    delete rooms[id];
    rooms.erase(id);

    id_assigner.releaseID(id);
}

void RoomManager::removeRoom (Room *room) {
    removeRoom(room->getManagerID());
}