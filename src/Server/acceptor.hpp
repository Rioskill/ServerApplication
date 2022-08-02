#ifndef ACCEPTOR
#define ACCEPTOR

#include <iostream>
#include "eventLoop.hpp"
#include "networkSocket.hpp"
#include "client.hpp"

#include "room.hpp"

#include "workerManager.hpp"

class Acceptor {
protected:
    EventLoop *loop;
    NetworkSocket &socket;

public:
    Acceptor (EventLoop *loop, NetworkSocket &socket): loop(loop), socket(socket) {}

    void accept();
};

class WorkerEchoAcceptor: Acceptor {
private:
    WorkerManager *workerManager;

    void process_message (Client *client, int worker_id);
    void close_connection (Client *client, int worker_id);
public:
    WorkerEchoAcceptor (EventLoop *loop, NetworkSocket &socket, WorkerManager *workerManager): Acceptor(loop, socket), workerManager(workerManager) {}

    void accept();
};

#endif