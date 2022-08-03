#ifndef ROOMMANAGER
#define ROOMMANAGER

#include <map>

#include "room.hpp"
#include "idAssigner.hpp"

class RoomManager {
private:
    std::map<int, Room*> rooms;
    ReuseIDAssigner id_assigner;

public:
    RoomManager() = default;
    ~RoomManager();

    Room *createRoom (Worker *worker);
    void removeRoom (int id);
    void removeRoom (Room *room);

    Room *getRoom (int id);
};

#endif