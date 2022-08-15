#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <map>
#include <chrono>
#include <functional>

#include "idAssigner.hpp"
#include "client.hpp"

class ClientManager {
    using time_pt = std::chrono::time_point<std::chrono::steady_clock>;
    using time_duration = std::chrono::duration<int>;
private:
    std::map<int, Client*> clients;
    std::map<int, time_pt> clients_last_request_times;

    ReuseIDAssigner id_assigner;
    EventLoop *loop;
    time_duration duration;
public:

    ClientManager (EventLoop *loop, time_duration duration): loop(loop), duration(duration) {}

    ~ClientManager() {
        for (auto it = clients.begin(); it != clients.end(); it++) {
            int id = it->first;
            delete clients[id];
        }
    }

    Client *createClient (const File &file) {
        int id = id_assigner.assignID();
        clients.emplace(id, new Client(file, loop, id));

        return clients[id];
    }

    int addClient (Client *client) {
        int id = id_assigner.assignID();
        clients.emplace(id, client);
        client->setManagerID(id);

        return id;
    }

    void removeClient (int id) {

        std::cout << "removing client with id: " << id << std::endl;

        Client *client = clients[id];

        client->remove_schedules_from(loop);

        client->close();

        // loop->schedule([client]{
        //     delete client;
        // });

        clients.erase(id);

        id_assigner.releaseID(id);
    }

    void removeClient (Client *client) {
        removeClient(client->getManagerID());
    }

    void updateTimeout (int id) {
        clients_last_request_times[id] = std::chrono::steady_clock::now();
    }

    void updateTimeout (Client *client) {
        updateTimeout(client->getManagerID());
    }

    void scheduleCheckTimeout(int id) {
        loop->schedule([this, id](){
            checkTimeout(id);
        });
    }

    void scheduleCheckTimeout(Client *client) {
        scheduleCheckTimeout(client->getManagerID());
    }

    void checkTimeout (int id) {
        time_pt now = std::chrono::steady_clock::now();
        if (now >= clients_last_request_times[id] + duration) {
            std::cout << "should delete client with id: " << id << std::endl;
            removeClient(id);
        } else {
            scheduleCheckTimeout(id);
        }
    }
};

#endif