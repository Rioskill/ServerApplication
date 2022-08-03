#ifndef CONNECTIONPROCESSOR
#define CONNECTIONPROCESSOR

#include "workerManager.hpp"
#include "room.hpp"
#include "client.hpp"

class ConnectionProcessor {
public:
    virtual void process(int client_fd) = 0;
};

class RoomEchoConnectionProcessor: public ConnectionProcessor {
private:
    WorkerManager *workerManager;
    Room *room;
    EventLoop *loop;

    void process_message (Client *client, int client_id);
    void close_connection (Client *client, int client_id);

public:
    RoomEchoConnectionProcessor (WorkerManager *workerManager, EventLoop *loop);
    ~RoomEchoConnectionProcessor();

    void process(int client_fd) override;
};

#endif