#ifndef ROOMMANAGER
#define ROOMMANAGER

#include <map>

#include "room.hpp"
#include "idAssigner.hpp"

class RoomManager {
private:
    std::map<int, Room*> rooms;
    IdAssigner id_assigner;

public:
    RoomManager ();
    ~RoomManager();

    int createRoom(Worker *worker, int worker_id);
    void removeRoom(int id);
};

#endif