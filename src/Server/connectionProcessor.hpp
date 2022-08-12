#ifndef CONNECTIONPROCESSOR_H
#define CONNECTIONPROCESSOR_H

#include "workerManager.hpp"
#include "roomManager.hpp"
#include "client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "clientManager.hpp"

#include <map>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <functional>

#include "urls.hpp"

class ConnectionProcessor {
public:
    virtual void process(Client *client) = 0;
};

class BasicEchoConnectionProcessor: public ConnectionProcessor {
private:
    EventLoop *loop;
    
public:
    BasicEchoConnectionProcessor (EventLoop *loop): loop(loop) {}

    void process (Client *client) override;
};

class HttpProcessor: public ConnectionProcessor {
private:
    EventLoop *loop;
    ClientManager *clientManager;

    void respond (Client *client, const HttpRequest &request);

public:
    HttpProcessor (EventLoop *loop, ClientManager *clientManager): loop(loop), clientManager(clientManager){}

    void process (Client *client) override;
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

    void process (Client *client) override;
};

#endif