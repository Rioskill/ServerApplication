#ifndef CONNECTIONPROCESSOR
#define CONNECTIONPROCESSOR

#include "workerManager.hpp"
#include "roomManager.hpp"
#include "client.hpp"

#include <map>
#include <iomanip>

class ConnectionProcessor {
public:
    virtual void process(int client_fd) = 0;
};

class RoomEchoConnectionProcessor: public ConnectionProcessor {
private:

    std::map<std::string, int> room_ids;

    WorkerManager *workerManager;
    RoomManager *roomManager;
    EventLoop *loop;

    void process_message (Room *room, Client *client);
    void close_connection (Room *room, Client *client);

    Room *connect_to_room (const std::string &room_name);

    std::string get_room_name (int room_id);

public:
    RoomEchoConnectionProcessor (WorkerManager *workerManager, RoomManager *roomManager, EventLoop *loop);
    ~RoomEchoConnectionProcessor();

    void process(int client_fd) override;
};

#endif