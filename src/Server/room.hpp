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
    Room (Worker *worker, int managerID);

    void setManagerID (int managerID);
    int getManagerID();
    
    Worker *getWorker();

    int getWorkerManagerId();

    int addClient (Client *client);

    void removeClient (int id);
    void removeClient (Client *client);

    void broadcast (unsigned int bytes, void *message, std::function<void()> cb);

    int clientsCount();
    bool empty();
};


#endif