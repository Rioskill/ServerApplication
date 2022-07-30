#ifndef ACCEPTOR
#define ACCEPTOR

#include <iostream>
#include "eventLoop.hpp"
#include "networkSocket.hpp"
#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"

#include "worker.hpp"

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
    Worker *worker;

public:
    WorkerEchoAcceptor (EventLoop *loop, NetworkSocket &socket, Worker *worker): Acceptor(loop, socket), worker(worker) {}

    void accept();
};

#endif