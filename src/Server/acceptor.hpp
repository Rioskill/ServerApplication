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

    Room *room;

    void process_message (Client *client, int worker_id);
    void close_connection (Client *client, int worker_id);
public:
    WorkerEchoAcceptor (EventLoop *loop, NetworkSocket &socket, WorkerManager *workerManager): Acceptor(loop, socket), workerManager(workerManager) {
        int worker_id = workerManager->createWorker();
        Worker *worker = workerManager->getWorker(worker_id);

        room = new Room(worker, worker_id);

        workerManager->startWorker(worker_id);
    }

    ~WorkerEchoAcceptor() {
        workerManager->stopWorker(room->getWorkerId());
        workerManager->removeWorker(room->getWorkerId());

        delete room;
    }

    void accept();
};

#endif